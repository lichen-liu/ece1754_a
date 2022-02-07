/*
 * A boiler plate starter file for using ROSE
 *
 * Input: sequential C/C++ code
 * Output: same C/C++ code 
 * 
 * ROSE API: http://rosecompiler.org/ROSE_HTML_Reference/
 * Tutorial: /u/course/ece1754/rose-0.11.63.0/projects/autoParallelization/
 *
 */

#include "rose.h"
#include <iostream>
#include <unordered_map>
#include <algorithm>

namespace {

std::string get_SgNode_string(SgNode *node) {
    SgLocatedNode *located = isSgLocatedNode(node);
    ROSE_ASSERT(located);
    Sg_File_Info *file_info = located->get_file_info();
    return file_info->get_physical_filename() + " " + std::to_string(file_info->get_line()) + ":" + std::to_string(file_info->get_col()) +
        " " + node->class_name();
}

SgInitializedName* is_loop_analyzable(SgNode *for_loop_node, bool debug=true, bool verbose=false) {
    // Determine the “analyzable” loop
    // An analyzable loop is defined as a for loop that has an induction variable (call it i) with: 
    // a. One or more initialization statements, 
    // b. A single test expression that uses <, <=, > or >= operations,  
    // c. A single increment expression in the form i=i+c, i=i-c, ++i, --i, i++ 
    //    or ++i, where c is a compile time constant, and 
    // d. The value of i is not changed in the loop

    if (debug) {
        std::cout << for_loop_node->unparseToString() << std::endl;
    }

    SgInitializedName *ivar = nullptr;
    SgExpression *lb = nullptr;
    SgExpression *ub = nullptr;
    SgExpression *step = nullptr;
    SgStatement *body = nullptr;

    // A canonical form is defined as:
    // a. one initialization statement
    // b. a test expression
    // c. an increment expression
    // d. loop index variable should be of an integer type

    // TODO: handwrite one that supports multiple initializations
    bool is_canonical = SageInterface::isCanonicalForLoop(for_loop_node, &ivar, &lb, &ub, &step, &body);
   
    if (debug) {
        std::cout << "  var:" << std::endl;
        SageInterface::printAST(ivar);

        std::cout << "  lb:" << std::endl;
        SageInterface::printAST(lb);

        std::cout << "  ub:" << std::endl;
        SageInterface::printAST(ub);

        std::cout << "  step:" << std::endl;
        SageInterface::printAST(step);
    }
    
    // If is_canonical is true, then need to check
    // 1. Whether increment step c is a compile time constant
    // 2. The value of the induction variable is not changed in the loop
    if (is_canonical) {
        if (debug) {
            std::cout << "  is_canonical=True" << std::endl;
        }

        // We want to check whether increment step c is a compile time constant.
        // For that to be true, step must be a constant integer value, so it must be a SgIntVal type
        // TODO: support constant propagation and folding
        SgIntVal *step_value = isSgIntVal(step);
        bool is_increment_step_constexpr = (step_value != nullptr);
        
        if (is_increment_step_constexpr) {
            if (debug) {
                std::cout << "    is_increment_step_constexpr=True, step_value=" << step_value->get_value() << std::endl;

                if (verbose) {
                    std::cout << "    body:" << std::endl;
                    SageInterface::printAST(body);
                }
            }

            // Check whether the induction variable is modified in the subtree
            // with the root that is the loop body of for_loop_node
            std::set<SgInitializedName*> read_vars;
            std::set<SgInitializedName*> write_vars;
            SageInterface::collectReadWriteVariables(body, read_vars, write_vars);
            if (debug) {
                if (verbose) {
                    std::cout << "    body read:" << std::endl;
                    for (auto r: read_vars) {
                        SageInterface::printAST(r);
                    }
                }
                std::cout << "    body write:" << std::endl;
                for (auto w: write_vars) {
                    SageInterface::printAST(w);
                }
            }

            bool is_induction_variable_unmodified = (write_vars.count(ivar) == 0);
            if (is_induction_variable_unmodified) {
                if (debug) {
                    std::cout << "      is_induction_variable_unmodified=true" << std::endl;
                }

                std::cout << "Analyzable! " << get_SgNode_string(for_loop_node) << std::endl;
                return ivar;
            }
        }
    }

    std::cout << "Not analyzable! " << get_SgNode_string(for_loop_node) << std::endl;
    return nullptr;
}

void determine_dependency_check_targets(SgNode *for_loop_node,
    const std::unordered_map<SgInitializedName*, SgForStatement*> &induction_variables,
    const std::unordered_map<SgForStatement*, SgInitializedName*> &analyzable_loops,
    bool debug=true) {
    SgForStatement *for_stmt = isSgForStatement(for_loop_node);
    ROSE_ASSERT(for_stmt);
    if (analyzable_loops.count(for_stmt) == 0) return;
    
    // Get all read write refs
    std::vector<SgNode*> read_refs;
    std::vector<SgNode*> write_refs;
    SageInterface::collectReadWriteRefs(for_stmt, read_refs, write_refs);

    // Filter only array refs
    auto mapfilter_SgPntrArrRefExp = [](const std::vector<SgNode*> &v) {
        std::vector<SgPntrArrRefExp*> tmp;
        std::transform(v.begin(), v.end(), std::back_inserter(tmp), [](SgNode *n){return isSgPntrArrRefExp(n);});
        std::vector<SgPntrArrRefExp*> res;
        std::copy_if(tmp.begin(), tmp.end(), std::back_inserter(res), [](SgPntrArrRefExp *n){return n;});
        return res;
    };
    std::vector<SgPntrArrRefExp*> read_array_refs = mapfilter_SgPntrArrRefExp(read_refs);
    std::vector<SgPntrArrRefExp*> write_array_refs = mapfilter_SgPntrArrRefExp(write_refs);

    if (debug) {
        std::cout << std::endl << std::endl;
        SageInterface::printAST(for_stmt);
        std::cout << std::endl;
        std::cout << "read_array_refs" << std::endl;
        for (auto r: read_array_refs) {
            SageInterface::printAST(r);
        }
        std::cout << "write_array_refs" << std::endl;
        for (auto w: write_array_refs) {
            SageInterface::printAST(w);
        }
    }

    // Deal with write dependency
    for (SgPntrArrRefExp *w_array_ref: write_array_refs) {
        SgStatement *enclosing_stmt = SageInterface::getEnclosingStatement(w_array_ref);
        SgScopeStatement *enclosing_loop_stmt = SageInterface::findEnclosingLoop(enclosing_stmt);
        SgForStatement *enclosing_for_stmt = isSgForStatement(enclosing_loop_stmt);
        if (enclosing_for_stmt == for_stmt) {
            if (debug) {
                std::cout << "Analyzing W [" << get_SgNode_string(w_array_ref) <<  
                    "] in [" << get_SgNode_string(for_stmt) << "]" << std::endl;
            }


            // Deal with write-write dependency

            // Deal with write-read dependency
        }
    }
}

void process_function_body(SgFunctionDefinition *defn) {
    SgBasicBlock *body = defn->get_body();
    std::cout << "Found a function" << std::endl;
    std::cout << "  " << defn->get_declaration()->get_qualified_name() << std::endl;
    // SageInterface::printAST(func);

    // Get all loops in the current function body
    Rose_STL_Container<SgNode*> loops = NodeQuery::querySubTree(body, V_SgForStatement); 
    if (loops.size() == 0) return;

    // Build a mapping between analyzable loop and its indunction variable
    std::unordered_map<SgInitializedName*, SgForStatement*> induction_variables;
    std::unordered_map<SgForStatement*, SgInitializedName*> analyzable_loops;
    for (Rose_STL_Container<SgNode*>::iterator iter = loops.begin(); iter != loops.end(); iter++) {
        SgNode *current_loop = *iter;
        
        std::cout << std::endl;
        std::cout << "Found a loop" << std::endl;
        SgInitializedName *ind_var = is_loop_analyzable(current_loop);

        if (ind_var) {
            SgForStatement *for_stmt = isSgForStatement(current_loop);
            ROSE_ASSERT(for_stmt);
            induction_variables.emplace(ind_var, for_stmt);
            analyzable_loops.emplace(for_stmt, ind_var);
        }

        std::cout << std::endl;
        // SageInterface::printAST(current_loop);
    }

    // Determine dependency check targets
    for (SgNode *current_loop: loops) {
        determine_dependency_check_targets(current_loop, induction_variables, analyzable_loops);
    }
}

}

int main (int argc, char *argv[]) {
    // Build a project
    SgProject *project = frontend(argc, argv);
    ROSE_ASSERT(project);

    // For each source file in the project
    SgFilePtrList &ptr_list = project->get_fileList();

    for (SgFilePtrList::iterator iter = ptr_list.begin(); iter!=ptr_list.end(); iter++) { 
        SgFile *sageFile = *iter; 
        SgSourceFile *sfile = isSgSourceFile(sageFile);
        ROSE_ASSERT(sfile);
        SgGlobal *root = sfile->get_globalScope();
        SgDeclarationStatementPtrList &declList = root->get_declarations();

        std::cout << "Found a file" << std::endl;
        std::cout << "  " << sfile->get_file_info()->get_filename() << std::endl;

        // For each function body in the scope
        for (SgDeclarationStatementPtrList::iterator p = declList.begin(); p != declList.end(); ++p) {
            SgFunctionDeclaration *func = isSgFunctionDeclaration(*p);
            if (func == nullptr) continue;
            SgFunctionDefinition *defn = func->get_definition();
            if (defn == nullptr) continue;
            // Ignore functions in system headers, Can keep them to test robustness
            if (defn->get_file_info()->get_filename() != sageFile->get_file_info()->get_filename()) continue;
            
            process_function_body(defn);
        } // end for-loop for declarations
    } //end for-loop for files

    std::cout << "Done ..." << std::endl;
    
    // Generate the source code
    return backend(project);
}

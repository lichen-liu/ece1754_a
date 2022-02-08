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
#include <unordered_set>
#include <algorithm>

namespace
{
    void serialize(SgNode *node, std::string &prefix, bool hasRemaining, std::ostringstream &out);
}

namespace
{
    // From sageInterface.C, modified to print only the current node
    // A special node in the AST text dump
    void serialize(SgTemplateArgumentPtrList &plist, std::string &prefix, bool hasRemaining, std::ostringstream &out)
    {
        out << prefix;
        out << (hasRemaining ? "|---" : "|___");

        // print address
        out << "@" << &plist << " "
            << "SgTemplateArgumentPtrList ";

        out << std::endl;
        for (size_t i = 0; i < plist.size(); i++)
        {
            bool n_hasRemaining = false;
            if (i + 1 < plist.size())
                n_hasRemaining = true;
            std::string suffix = hasRemaining ? "|   " : "    ";
            std::string n_prefix = prefix + suffix;
            serialize(plist[i], n_prefix, n_hasRemaining, out);
        }
    }

    // From sageInterface.C, modified to print only the current node
    void serialize(SgNode *node, std::string &prefix, bool hasRemaining, std::ostringstream &out)
    {
        // there may be NULL children!!
        //if (!node) return;

        out << prefix;
        // out << (hasRemaining ? "|---" : "|___");
        if (!node)
        {
            out << " NULL " << std::endl;
            return;
        }

        // print address
        out << "@" << node << " " << node->class_name() << " ";

        //optionally file info
        if (SgLocatedNode *lnode = isSgLocatedNode(node))
        {
            out << Rose::StringUtility::stripPathFromFileName(lnode->get_file_info()->get_filename()) << " " << lnode->get_file_info()->get_line() << ":" << lnode->get_file_info()->get_col();
        }

        // optionally  qualified name
        if (SgFunctionDeclaration *f = isSgFunctionDeclaration(node))
            out << " " << f->get_qualified_name();

        if (SgClassDeclaration *f = isSgClassDeclaration(node))
            out << " " << f->get_qualified_name();

        if (SgTypedefDeclaration *f = isSgTypedefDeclaration(node))
            out << " " << f->get_qualified_name();

        if (SgAdaPackageSpecDecl *f = isSgAdaPackageSpecDecl(node))
            out << " " << f->get_qualified_name();

        if (SgInitializedName *v = isSgInitializedName(node))
        {
            out << " " << v->get_qualified_name();
            out << " type@" << v->get_type();
            out << " initializer@" << v->get_initializer();
            //    type_set.insert (v->get_type());
        }

        // associated class, function and variable declarations
        if (SgTemplateInstantiationDecl *f = isSgTemplateInstantiationDecl(node))
            out << " template class decl@" << f->get_templateDeclaration();

        if (SgMemberFunctionDeclaration *f = isSgMemberFunctionDeclaration(node))
            out << " assoc. class decl@" << f->get_associatedClassDeclaration();

        if (SgConstructorInitializer *ctor = isSgConstructorInitializer(node))
        {
            out << " member function decl@" << ctor->get_declaration();
        }

        if (SgIntVal *v = isSgIntVal(node))
            out << " value=" << v->get_value() << " valueString=" << v->get_valueString();

        if (SgVarRefExp *var_ref = isSgVarRefExp(node))
            out << " init name@" << var_ref->get_symbol()->get_declaration() << " symbol name=" << var_ref->get_symbol()->get_name();

        if (SgMemberFunctionRefExp *func_ref = isSgMemberFunctionRefExp(node))
            out << " member func decl@" << func_ref->get_symbol_i()->get_declaration();

        if (SgTemplateInstantiationMemberFunctionDecl *cnode = isSgTemplateInstantiationMemberFunctionDecl(node))
            out << " template member func decl@" << cnode->get_templateDeclaration();

        if (SgFunctionRefExp *func_ref = isSgFunctionRefExp(node))
        {
            SgFunctionSymbol *sym = func_ref->get_symbol_i();
            out << " func decl@" << sym->get_declaration() << " func sym name=" << sym->get_name();
        }

        // base type of several types of nodes:
        if (SgTypedefDeclaration *v = isSgTypedefDeclaration(node))
        {
            out << " base_type@" << v->get_base_type();
            //    type_set.insert (v->get_base_type());
        }

        if (SgArrayType *v = isSgArrayType(node))
            out << " base_type@" << v->get_base_type();

        if (SgTypeExpression *v = isSgTypeExpression(node))
            out << " type@" << v->get_type();

        if (SgAdaAttributeExp *v = isSgAdaAttributeExp(node))
            out << " attribute@" << v->get_attribute();

        if (SgDeclarationStatement *v = isSgDeclarationStatement(node))
        {
            out << " first nondefining decl@" << v->get_firstNondefiningDeclaration();
            out << " defining decl@" << v->get_definingDeclaration();
        }

        // out << std::endl;

        std::vector<SgNode *> children = node->get_traversalSuccessorContainer();
        int total_count = children.size();
        int current_index = 0;

        // some Sg??PtrList are not AST nodes, not part of children , we need to handle them separatedly
        // we sum all children into single total_count to tell if there is remaining children.
        if (isSgTemplateInstantiationDecl(node))
            total_count += 1; // sn->get_templateArguments().size();

        // handling SgTemplateArgumentPtrList first
        if (SgTemplateInstantiationDecl *sn = isSgTemplateInstantiationDecl(node))
        {
            SgTemplateArgumentPtrList &plist = sn->get_templateArguments();
            bool n_hasRemaining = false;
            if (current_index + 1 < total_count)
                n_hasRemaining = true;
            current_index++;

            std::string suffix = hasRemaining ? "|   " : "    ";
            std::string n_prefix = prefix + suffix;
            out << std::endl;
            serialize(plist, n_prefix, n_hasRemaining, out);
        }

        // finish sucessors
        // for (size_t i = 0; i < children.size(); i++)
        // {
        //     bool n_hasRemaining = false;
        //     if (current_index + 1 < total_count)
        //         n_hasRemaining = true;
        //     current_index++;

        //     string suffix = hasRemaining ? "|   " : "    ";
        //     string n_prefix = prefix + suffix;
        //     serialize(children[i], n_prefix, n_hasRemaining, out);
        // }
    }

    std::string get_indent(int indent)
    {
        return std::string(2 * indent, ' ');
    }

    std::string to_string(SgNode *node)
    {
        std::ostringstream ss;
        std::string prefix;
        serialize(node, prefix, false, ss);
        return ss.str();
    }

    void print_ast(SgNode *root)
    {
        std::cout << std::endl;
        std::cout << "====================== DEBUG ======================" << std::endl;
        SageInterface::printAST(root);
        std::cout << "====================== DEBUG ======================" << std::endl;
        std::cout << std::endl;
    }

    SgInitializedName *get_array_from_ref(SgPntrArrRefExp *array_ref, bool debug = false, int indent = 0)
    {
        SgExpression *array_name_exp = nullptr;
        std::unique_ptr<std::vector<SgExpression *>> array_ref_subscripts = std::make_unique<std::vector<SgExpression *>>();
        std::vector<SgExpression *> *array_ref_subscripts_p = array_ref_subscripts.get();
        // What a stupid API??? You only need to modify a vector but why a double pointer?
        SageInterface::isArrayReference(array_ref, &array_name_exp, &array_ref_subscripts_p);

        SgInitializedName *array_name = SageInterface::convertRefToInitializedName(array_name_exp);
        ROSE_ASSERT(array_name);

        if (debug)
        {
            const std::string indent_str = get_indent(indent);
            const std::string inner_indent_str = get_indent(1 + indent);
            std::cout << indent_str << "array_name: " << to_string(array_name) << std::endl;
            std::cout << indent_str << "array: " << to_string(array_name_exp) << std::endl;
            std::cout << indent_str << "subscript:" << std::endl;
            for (SgExpression *s : *array_ref_subscripts)
            {
                std::cout << inner_indent_str << to_string(s) << std::endl;
            }
        }
        return array_name;
    }

    SgForStatement *find_common_ancestor_for_stmt(SgForStatement *n1, SgForStatement *n2, SgScopeStatement *nt, bool debug = false)
    {
        if (n1 == n2)
            return n1;
        if (SageInterface::isAncestor(n1, n2))
            return n1;
        if (SageInterface::isAncestor(n2, n1))
            return n2;

        auto ancestor_visitor = [nt, debug](SgForStatement *from, std::function<bool(SgForStatement *)> worker)
        {
            if (debug)
            {
                std::cout << "ancestor traversal: " << to_string(from) << std::endl;
            }
            SgStatement *cur_node = SageInterface::getEnclosingStatement(from);
            while (cur_node != nt)
            {
                if (debug)
                {
                    std::cout << "  " << to_string(cur_node) << std::endl;
                }

                if (SgScopeStatement *enclosing_loop_stmt = SageInterface::findEnclosingLoop(cur_node))
                {
                    if (SgForStatement *enclosing_for_stmt = isSgForStatement(enclosing_loop_stmt))
                    {
                        if (!worker(enclosing_for_stmt))
                        {
                            return;
                        }
                    }
                }

                ROSE_ASSERT(SageInterface::getEnclosingScope(cur_node) != cur_node);
                cur_node = SageInterface::getEnclosingScope(cur_node);
            }
        };

        // Do a tree traversal upwards to find all n1 ancestors
        std::unordered_set<SgForStatement *> n1_ancestors;
        ancestor_visitor(n1, [&n1_ancestors](SgForStatement *enclosing_for_stmt)
                         {
                             n1_ancestors.insert(enclosing_for_stmt);
                             return true;
                         });

        // Do a tree traversal upwards to find all n2 ancestors
        SgForStatement *common_ancestor = nullptr;
        ancestor_visitor(n2, [&n1_ancestors, &common_ancestor](SgForStatement *enclosing_for_stmt)
                         {
                             if (n1_ancestors.count(enclosing_for_stmt))
                             {
                                 common_ancestor = enclosing_for_stmt;
                                 // early exit as soon as we hit an n1 ancestor
                                 return false;
                             }
                             return true;
                         });

        return common_ancestor;
    }

    SgForStatement *get_enclosing_for_stmt(SgPntrArrRefExp *array_ref)
    {
        SgStatement *enclosing_stmt = SageInterface::getEnclosingStatement(array_ref);
        SgScopeStatement *enclosing_loop_stmt = SageInterface::findEnclosingLoop(enclosing_stmt);
        if (enclosing_loop_stmt)
        {
            return isSgForStatement(enclosing_loop_stmt);
        }

        return nullptr;
    }

    SgInitializedName *is_loop_analyzable(SgNode *for_loop_node, bool debug = true, bool verbose = false)
    {
        // Determine the “analyzable” loop
        // An analyzable loop is defined as a for loop that has an induction variable (call it i) with:
        // a. One or more initialization statements,
        // b. A single test expression that uses <, <=, > or >= operations,
        // c. A single increment expression in the form i=i+c, i=i-c, ++i, --i, i++
        //    or ++i, where c is a compile time constant, and
        // d. The value of i is not changed in the loop

        if (debug)
        {
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

        if (debug)
        {
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
        if (is_canonical)
        {
            if (debug)
            {
                std::cout << "  is_canonical=True" << std::endl;
            }

            // We want to check whether increment step c is a compile time constant.
            // For that to be true, step must be a constant integer value, so it must be a SgIntVal type
            // TODO: support constant propagation and folding
            SgIntVal *step_value = isSgIntVal(step);
            bool is_increment_step_constexpr = (step_value != nullptr);

            if (is_increment_step_constexpr)
            {
                if (debug)
                {
                    std::cout << "    is_increment_step_constexpr=True, step_value=" << step_value->get_value() << std::endl;

                    if (verbose)
                    {
                        std::cout << "    body:" << std::endl;
                        SageInterface::printAST(body);
                    }
                }

                // Check whether the induction variable is modified in the subtree
                // with the root that is the loop body of for_loop_node
                std::set<SgInitializedName *> read_vars;
                std::set<SgInitializedName *> write_vars;
                SageInterface::collectReadWriteVariables(body, read_vars, write_vars);
                if (debug)
                {
                    if (verbose)
                    {
                        std::cout << "    body read:" << std::endl;
                        for (auto r : read_vars)
                        {
                            SageInterface::printAST(r);
                        }
                    }
                    std::cout << "    body write:" << std::endl;
                    for (auto w : write_vars)
                    {
                        SageInterface::printAST(w);
                    }
                }

                bool is_induction_variable_unmodified = (write_vars.count(ivar) == 0);
                if (is_induction_variable_unmodified)
                {
                    if (debug)
                    {
                        std::cout << "      is_induction_variable_unmodified=true" << std::endl;
                    }

                    std::cout << "Analyzable! " << to_string(for_loop_node) << std::endl;
                    return ivar;
                }
            }
        }

        std::cout << "Not analyzable! " << to_string(for_loop_node) << std::endl;
        return nullptr;
    }

    // Check over an entire scope. Do not call this function on multiple scopes that overlap
    void determine_dependency_check_targets(SgScopeStatement *scope_stmt,
                                            const std::unordered_map<SgInitializedName *, SgForStatement *> &induction_variables,
                                            const std::unordered_map<SgForStatement *, SgInitializedName *> &analyzable_loops,
                                            bool debug = true)
    {
        // Get all read write refs
        std::vector<SgNode *> read_refs;
        std::vector<SgNode *> write_refs;
        SageInterface::collectReadWriteRefs(scope_stmt, read_refs, write_refs);

        // Filter only array refs
        auto mapfilter_SgPntrArrRefExp = [](const std::vector<SgNode *> &v)
        {
            std::vector<SgPntrArrRefExp *> tmp;
            std::transform(v.begin(), v.end(), std::back_inserter(tmp), [](SgNode *n)
                           { return isSgPntrArrRefExp(n); });
            std::vector<SgPntrArrRefExp *> res;
            std::copy_if(tmp.begin(), tmp.end(), std::back_inserter(res), [](SgPntrArrRefExp *n)
                         { return n; });
            return res;
        };
        std::vector<SgPntrArrRefExp *> read_array_refs = mapfilter_SgPntrArrRefExp(read_refs);
        std::vector<SgPntrArrRefExp *> write_array_refs = mapfilter_SgPntrArrRefExp(write_refs);

        if (debug)
        {
            std::cout << std::endl;
            print_ast(scope_stmt);
            std::cout << "In scope: " << to_string(scope_stmt) << std::endl;
            std::cout << "read_array_refs:" << std::endl;
            for (auto r : read_array_refs)
            {
                SageInterface::printAST(r);
            }
            std::cout << "write_array_refs:" << std::endl;
            for (auto w : write_array_refs)
            {
                SageInterface::printAST(w);
            }
        }

        // Deal with write dependency
        for (auto w_array_ref_it = write_array_refs.begin(); w_array_ref_it != write_array_refs.end(); ++w_array_ref_it)
        {
            SgPntrArrRefExp *w_array_ref = *w_array_ref_it;
            if (debug)
            {
                std::cout << "Analyzing Write " << to_string(w_array_ref) << std::endl;
            }

            SgForStatement *w_array_ref_enclosing_for_stmt = get_enclosing_for_stmt(w_array_ref);
            // Check w_array_ref is inside a for loop
            if (w_array_ref_enclosing_for_stmt == nullptr)
                continue;

            // Deal with write array ref that's at the current for stmt scope
            // if (enclosing_for_stmt != for_stmt)
            //     continue;

            SgInitializedName *w_array_name = get_array_from_ref(w_array_ref, debug, 1);

            // Deal with write-write dependency
            for (auto target_w_array_ref_it = w_array_ref_it + 1; target_w_array_ref_it != write_array_refs.end(); ++target_w_array_ref_it)
            {
                SgPntrArrRefExp *target_w_array_ref = *target_w_array_ref_it;
                if (debug)
                {
                    const std::string indent_str = get_indent(1);
                    std::cout << indent_str << "Write Target " << to_string(target_w_array_ref) << std::endl;
                }

                // Find write-write dependency on the same array name
                SgInitializedName *target_w_array_name = get_array_from_ref(target_w_array_ref, debug, 2);
                if (target_w_array_name != w_array_name)
                    continue;
                if (debug)
                {
                    const std::string indent_str = get_indent(3);
                    std::cout << indent_str << "Name Matching.." << std::endl;
                }

                SgForStatement *target_w_array_ref_enclosing_for_stmt = get_enclosing_for_stmt(target_w_array_ref);
                // Check w_array_ref is inside a for loop
                if (target_w_array_ref_enclosing_for_stmt == nullptr)
                    continue;

                // Find common ancestor
                SgForStatement *ancestor_for_stmt = find_common_ancestor_for_stmt(w_array_ref_enclosing_for_stmt, target_w_array_ref_enclosing_for_stmt, scope_stmt);
                if (debug)
                {
                    const std::string indent_str = get_indent(3);
                    std::cout << indent_str << "Ancestor for_stmt: " << to_string(ancestor_for_stmt) << std::endl;
                }
            }

            // Deal with write-read dependency
        }
    }

    void process_function_body(SgFunctionDefinition *defn)
    {
        SgBasicBlock *body = defn->get_body();
        std::cout << "Found a function" << std::endl;
        std::cout << "  " << defn->get_declaration()->get_qualified_name() << std::endl;
        // SageInterface::printAST(func);

        // Get all loops in the current function body
        Rose_STL_Container<SgNode *> loops = NodeQuery::querySubTree(body, V_SgForStatement);
        if (loops.size() == 0)
            return;

        // Build a mapping between analyzable loop and its indunction variable
        std::unordered_map<SgInitializedName *, SgForStatement *> induction_variables;
        std::unordered_map<SgForStatement *, SgInitializedName *> analyzable_loops;
        for (Rose_STL_Container<SgNode *>::iterator iter = loops.begin(); iter != loops.end(); iter++)
        {
            SgNode *current_loop = *iter;

            std::cout << std::endl;
            std::cout << "Found a loop" << std::endl;
            SgInitializedName *ind_var = is_loop_analyzable(current_loop);

            if (ind_var)
            {
                SgForStatement *for_stmt = isSgForStatement(current_loop);
                ROSE_ASSERT(for_stmt);
                induction_variables.emplace(ind_var, for_stmt);
                analyzable_loops.emplace(for_stmt, ind_var);
            }

            std::cout << std::endl;
            // SageInterface::printAST(current_loop);
        }

        // Determine dependency check targets
        determine_dependency_check_targets(body, induction_variables, analyzable_loops);
    }

}

int main(int argc, char *argv[])
{
    // Build a project
    SgProject *project = frontend(argc, argv);
    ROSE_ASSERT(project);

    // For each source file in the project
    SgFilePtrList &ptr_list = project->get_fileList();

    for (SgFilePtrList::iterator iter = ptr_list.begin(); iter != ptr_list.end(); iter++)
    {
        SgFile *sageFile = *iter;
        SgSourceFile *sfile = isSgSourceFile(sageFile);
        ROSE_ASSERT(sfile);
        SgGlobal *root = sfile->get_globalScope();
        SgDeclarationStatementPtrList &declList = root->get_declarations();

        std::cout << "Found a file" << std::endl;
        std::cout << "  " << sfile->get_file_info()->get_filename() << std::endl;

        // For each function body in the scope
        for (SgDeclarationStatementPtrList::iterator p = declList.begin(); p != declList.end(); ++p)
        {
            SgFunctionDeclaration *func = isSgFunctionDeclaration(*p);
            if (func == nullptr)
                continue;
            SgFunctionDefinition *defn = func->get_definition();
            if (defn == nullptr)
                continue;
            // Ignore functions in system headers, Can keep them to test robustness
            if (defn->get_file_info()->get_filename() != sageFile->get_file_info()->get_filename())
                continue;

            process_function_body(defn);
        } // end for-loop for declarations
    }     //end for-loop for files

    std::cout << "Done ..." << std::endl;

    // Generate the source code
    return backend(project);
}

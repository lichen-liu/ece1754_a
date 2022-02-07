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

bool is_loop_analyzable(SgNode *for_loop_node) {
    // SgForStatement* for_loop_stmt = dynamic_cast<SgForStatement>(for_loop_node);
    // ROSE_ASSERT(for_loop_stmt);
    
    // SgStatement *test_stmt = for_loop_stmt->get_test();

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
    bool is_canonical = SageInterface::isCanonicalForLoop(for_loop_node, &ivar, &lb, &ub, &step, &body);
   
    std::cout << "  var:" << std::endl;
    SageInterface::printAST(ivar);

    std::cout << "  lb:" << std::endl;
    SageInterface::printAST(lb);

    std::cout << "  ub:" << std::endl;
    SageInterface::printAST(ub);

    std::cout << "  step:" << std::endl;
    SageInterface::printAST(step);

    if (is_canonical) {
        
    }
    // The “analyzable” loops. An analyzable loop is defined as a for loop that has an induction variable (call it i) with: 
    // a. one or more initialization statements, 
    // b. a single test expression that uses <, <=, > or >= operations,  
    // c. a single increment expression in the form i=i+c, i=i-c, ++i, --i, i++ 
    // or ++i, where c is a compile time constant, and 
    // d. the value of i is not changed in the loop
    
    // If is_canonical is true
    // 1. only need to check whether increment step c is a compile time constant
    // 2. the value of the induction variable is not changed in the loop


    return true;
}

int main (int argc, char *argv[]) {
    // Build a project
    SgProject *project = frontend(argc,argv);
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
            SgBasicBlock *body = defn->get_body();  

            std::cout << "Found a function" << std::endl;
            std::cout << "  " << func->get_qualified_name() << std::endl;
            // SageInterface::printAST(func);

            // For each loop 
            Rose_STL_Container<SgNode*> loops = NodeQuery::querySubTree(defn,V_SgForStatement); 
            if (loops.size() == 0) continue;

            for (Rose_STL_Container<SgNode*>::iterator iter = loops.begin(); iter != loops.end(); iter++) {
                SgNode *current_loop = *iter;
                
                std::cout << "Found a loop" << std::endl;
                is_loop_analyzable(current_loop);
                // SageInterface::printAST(current_loop);

            } // end for-loop for loops
        } // end for-loop for declarations
    } //end for-loop for files

    std::cout << "Done ..." << std::endl;
    
    // Generate the source code
    return backend(project);
}

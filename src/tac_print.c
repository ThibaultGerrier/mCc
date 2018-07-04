#include "mCc/tac_print.h"
#include <assert.h>


#define LABEL_SIZE 64

static void print_dot_begin(FILE *out, char* name)
{
    assert(out);

    fprintf(out, "digraph \"%s\" {\n",name);
    fprintf(out, "\tnodesep=0.6\n");
}

static void print_dot_end(FILE *out)
{
    assert(out);

    fprintf(out, "}\n");
}

static void print_dot_node(FILE *out, const void *node, const char *label)
{
    assert(out);
    assert(node);
    assert(label);

    fprintf(out, "\t\"%p\" [shape=box, label=\"%s\"];\n", node, label);
}

static void print_dot_node_diamond(FILE *out, const void *node, const char *label)
{
    assert(out);
    assert(node);
    assert(label);

    fprintf(out, "\t\"%p\" [shape=diamond, label=\"%s\"];\n", node, label);
}

static void print_dot_node_label(FILE *out, int label)
{
    assert(out);
    assert(label);

    fprintf(out, "\t\"Label%d\" [shape=box, label=\"Label%d\"];\n", label,label);
}

static void print_dot_edge(FILE *out, const void *src_node,
                           const void *dst_node, const char *label)
{
    assert(out);
    assert(src_node);
    assert(dst_node);
    assert(label);

    fprintf(out, "\t\"%p\" -> \"%p\" [label=\"%s\"];\n", src_node, dst_node,
            label);
}

static void print_dot_edge_from_label(FILE *out,
                           const void *dst_node, int label)
{
    assert(out);
    assert(dst_node);
    assert(label);

    fprintf(out, "\t\"Label%d\" -> \"%p\" [label=\"%s\"];\n", label, dst_node,
            "");
}


static void print_dot_edge_label(FILE *out, const void *src_node,
                           int label, bool ifz)
{
    assert(out);
    assert(src_node);
    assert(label);
    if(ifz==true){
        fprintf(out, "\t\"%p\" -> \"Label%d\" [label=\"%s\"];\n", src_node, label,"YES");
    }else{
        fprintf(out, "\t\"%p\" -> \"Label%d\" [label=\"%s\"];\n", src_node, label,"");
    }


}


static void check_dot_edge(FILE *out, mCc_tac_node node,const char *label)
{
    if(node->next!=NULL){
        print_dot_edge(out,node,node->next,label);
    }
}

const char *mCc_get_unary_op(enum mCc_ast_unary_op op)
{
    /**** unary operations ****/
    switch (op) {
        case MCC_AST_UNARY_OP_NOT: return "!";
        case MCC_AST_UNARY_OP_MINUS: return "-";
    }
    return "unknown unary op";
}


void mCc_tac_print_dot(FILE *out, mCc_tac_node head)
{
    assert(out);
    assert(head);


    mCc_tac_node p;
    p = head;
    while (p != NULL) {
        switch (p->type) {
            case TAC_LINE_TYPE_SIMPLE: {
                char *var1 = mCc_tac_get_tac_var(p->type_simple.arg0);
                char *var2 = mCc_tac_get_tac_var(p->type_simple.arg1);
                char label[LABEL_SIZE] = { 0 };
                snprintf(label, sizeof(label), "%s = %s\n", var1, var2);
                print_dot_node(out,p,label);
                if(p->next->type==TAC_LINE_TYPE_LABEL){
                    print_dot_edge_label(out,p,p->next->type_label.label_name,false);
                }else{
                    check_dot_edge(out,p,"");
                }

                free(var1);
                free(var2);
                break;
            }
            case TAC_LINE_TYPE_DOUBLE: {
                char *var1 = mCc_tac_get_tac_var(p->type_double.arg0);
                char *var2 = mCc_tac_get_tac_var(p->type_double.arg1);
                char *var3 = mCc_tac_get_tac_var(p->type_double.arg2);
                char label[LABEL_SIZE] = { 0 };
                snprintf(label, sizeof(label), "%s = %s %s %s\n", var1, var2,
                         mCc_tac_get_binary_op(p->type_double.op.op,
                                               p->type_double.op.type),
                         var3);
                print_dot_node(out,p,label);
                if(p->next->type==TAC_LINE_TYPE_LABEL){
                    print_dot_edge_label(out,p,p->next->type_label.label_name,false);
                }else{
                    check_dot_edge(out,p,"");
                }
                free(var1);
                free(var2);
                free(var3);
                break;
            }
            case TAC_LINE_TYPE_UNARY: {
                char *var1 = mCc_tac_get_tac_var(p->type_unary.arg0);
                char *var2 = mCc_tac_get_tac_var(p->type_unary.arg1);
                char label[LABEL_SIZE] = { 0 };
                snprintf(label, sizeof(label), "%s = %s %s\n", var1,
                         mCc_get_unary_op(p->type_unary.op.op), var2);
                print_dot_node(out,p,label);
                if(p->next->type==TAC_LINE_TYPE_LABEL){
                    print_dot_edge_label(out,p,p->next->type_label.label_name,false);
                }else{
                    check_dot_edge(out,p,"");
                }
                free(var1);
                free(var2);
                break;
            }
            case TAC_LINE_TYPE_CALL: {
                char label[LABEL_SIZE] = {0};
                snprintf(label, sizeof(label), "CALL(%s)\n", p->type_call.name);
                print_dot_node(out, p, label);
                if(p->next->type==TAC_LINE_TYPE_LABEL){
                    print_dot_edge_label(out,p,p->next->type_label.label_name,false);
                }else{
                    check_dot_edge(out,p,"");
                }
                break;
            }
            case TAC_LINE_TYPE_POP: {
                char *var1 = mCc_tac_get_tac_var(p->type_pop.var);
                char label[LABEL_SIZE] = {0};
                snprintf(label, sizeof(label),  "%s = POP\n", var1);
                print_dot_node(out,p,label);
                if(p->next->type==TAC_LINE_TYPE_LABEL){
                    print_dot_edge_label(out,p,p->next->type_label.label_name,false);
                }else{
                    check_dot_edge(out,p,"");
                }

                free(var1);
                break;
            }
            case TAC_LINE_TYPE_POP_RETURN: {
                char *var1 = mCc_tac_get_tac_var(p->type_pop.var);
                char label[LABEL_SIZE] = {0};
                snprintf(label, sizeof(label),  "%s = POP\n", var1);
                print_dot_node(out,p,label);
                if(p->next->type==TAC_LINE_TYPE_LABEL){
                    print_dot_edge_label(out,p,p->next->type_label.label_name,false);
                }else{
                    check_dot_edge(out,p,"");
                }

                free(var1);
                break;
            }
            case TAC_LINE_TYPE_PUSH: {
                char *var1 = mCc_tac_get_tac_var(p->type_push.var);
                char label[LABEL_SIZE] = {0};
                snprintf(label, sizeof(label),  "%s = PUSH\n", var1);
                print_dot_node(out,p,label);
                if(p->next->type==TAC_LINE_TYPE_LABEL){
                    print_dot_edge_label(out,p,p->next->type_label.label_name,false);
                }else{
                    check_dot_edge(out,p,"");
                }
                free(var1);
                break;
            }
            case TAC_LINE_TYPE_RETURN: {
                char *var1 = mCc_tac_get_tac_var(p->type_return.var);
                char label[LABEL_SIZE] = {0};
                snprintf(label, sizeof(label),  "%s = RETURN\n", var1);
                print_dot_node(out,p,label);
                if(p->next->type==TAC_LINE_TYPE_LABEL){
                    print_dot_edge_label(out,p,p->next->type_label.label_name,false);
                }else{
                    check_dot_edge(out,p,"");
                }
                free(var1);
                break;
            }
            case TAC_LINE_TYPE_IFZ: {
                char *var = mCc_tac_get_tac_var(p->type_ifz.var);
                char label[LABEL_SIZE] = {0};
                snprintf(label, sizeof(label),  "IF-Zero %s GOTO Label%d\n", var,
                p->type_ifz.jump_label_name);
                print_dot_node_diamond(out,p,label);
                check_dot_edge(out,p,"NO");
                print_dot_edge_label(out,p,p->type_ifz.jump_label_name,true);
                free(var);
                break;
            }
            case TAC_LINE_TYPE_DECL_ARRAY: {
                char *var1 = mCc_tac_get_tac_var(p->type_decl_array.var);
                char label[LABEL_SIZE] = {0};
                snprintf(label, sizeof(label),  "%s[%d]\n", var1, p->type_decl_array.var.array);
                print_dot_node(out,p,label);
                if(p->next->type==TAC_LINE_TYPE_LABEL){
                    print_dot_edge_label(out,p,p->next->type_label.label_name,false);
                }else{
                    check_dot_edge(out,p,"");
                }
                free(var1);
                break;
            }

            case TAC_LINE_TYPE_IDEN_ARRAY: {
                char *var1 = mCc_tac_get_tac_var(p->type_array_iden.arr);
                char *var2 = mCc_tac_get_tac_var(p->type_array_iden.loc);
                char *var3 = mCc_tac_get_tac_var(p->type_array_iden.var);
                char label[LABEL_SIZE] = {0};
                snprintf(label, sizeof(label),   "%s = %s[%s]\n", var3, var1, var2);
                print_dot_node(out,p,label);
                if(p->next->type==TAC_LINE_TYPE_LABEL){
                    print_dot_edge_label(out,p,p->next->type_label.label_name,false);
                }else{
                    check_dot_edge(out,p,"");
                }
                free(var1);
                free(var2);
                free(var3);
                break;
            }
            case TAC_LINE_TYPE_ASSIGNMENT_ARRAY: {
                char *var1 = mCc_tac_get_tac_var(p->type_assign_array.arr);
                char *var2 = mCc_tac_get_tac_var(p->type_assign_array.loc);
                char *var3 = mCc_tac_get_tac_var(p->type_assign_array.var);

                char label[LABEL_SIZE] = {0};
                snprintf(label, sizeof(label),    "%s[%s] = %s\n", var1, var2, var3);
                print_dot_node(out,p,label);
                if(p->next->type==TAC_LINE_TYPE_LABEL){
                    print_dot_edge_label(out,p,p->next->type_label.label_name,false);
                }else{
                    check_dot_edge(out,p,"");
                }
                free(var1);
                free(var2);
                free(var3);
                break;
            }

            case TAC_LINE_TYPE_LABEL: {
                print_dot_node_label(out,p->type_label.label_name);
                if(p->next!=NULL){
                    print_dot_edge_from_label(out,p->next,p->type_label.label_name);
                }
                break;
            }
            case TAC_LINE_TYPE_JUMP: {
                char label[LABEL_SIZE] = {0};
                snprintf(label, sizeof(label), "GOTO Label%d\n", p->type_jump.jump_name);
                print_dot_node(out,p,label);
                print_dot_edge_label(out,p,p->type_jump.jump_name,false);
                break;
            }
            case TAC_LINE_TYPE_LABELFUNC:
                print_dot_begin(out,p->type_label_func.func_name);
                char label[LABEL_SIZE] = {0};
                snprintf(label, sizeof(label), "START %s", p->type_label_func.func_name);
                print_dot_node(out,p,label);
                if(p->next->type==TAC_LINE_TYPE_LABEL){
                    print_dot_edge_label(out,p,p->next->type_label.label_name,false);
                }else{
                    check_dot_edge(out,p,"");
                }
                break;
            case TAC_LINE_TYPE_LABELFUNC_END: {
                char label[LABEL_SIZE] = {0};
                snprintf(label, sizeof(label), "END %s", p->type_label_func.func_name);
                print_dot_node(out, p, label);
                print_dot_end(out);
                break;
            }
            case TAC_LINE_TYPE_BEGIN: break;
            default: fprintf(out, "error %d\n", p->type); break;
        }
        p = p->next;
    }

}
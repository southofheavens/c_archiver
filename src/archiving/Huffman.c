#include <Huffman.h>
#include <stack.h>
#include <darray.h>

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

/* */

typedef struct huffman_tree_node huffman_tree_node;

struct huffman_tree_node
{
    huffman_tree_node *parent;
    huffman_tree_node *left_child;
    huffman_tree_node *right_child;
    size_t             frequency;
    uint8_t               letter;
    bool               is_fictitious;
};

typedef struct _huffman_tree
{
    darray *nodes;
} huffman_tree;


/*  */


huffman_tree_node *
huffman_tree_node_create
(
    void
)
{
    huffman_tree_node *node = (huffman_tree_node *)malloc(sizeof(huffman_tree_node));
    if (node == NULL)
    {
        perror("");
        exit(1);
    }

    *node = (huffman_tree_node)
    {
        .parent        = NULL,
        .left_child    = NULL,
        .right_child   = NULL,
        .frequency     = 1,
        .letter        = 0,
        .is_fictitious = true
    };

    return node;
}

int 
huffman_tree_node_compare
(
    const void *first,
    const void *second
)
{
    huffman_tree_node *first_node = *((huffman_tree_node **)first);
    huffman_tree_node *second_node = *((huffman_tree_node **)second);
    return ((first_node->letter == second_node->letter) &&
        (first_node->is_fictitious == second_node->is_fictitious) ? 0 : 1);
}   


/* */


huffman_tree *
huffman_tree_create
(
    void
)
{
    huffman_tree *tree = (huffman_tree *)malloc(sizeof(huffman_tree));
    if (tree == NULL)
    {
        perror("");
        exit(1);
    }

    tree->nodes = darray_create(sizeof(huffman_tree_node *));

    huffman_tree_node *art_node = huffman_tree_node_create();

    darray_append(tree->nodes, art_node);

    return tree;
}

void
huffman_tree_free
(
    huffman_tree *tree
)
{
    for (size_t i = 0; i < darray_size(tree->nodes); ++i) {
        free(darray_at(tree->nodes, i, huffman_tree_node *));
    }

    darray_free(tree->nodes);
    free(tree);
}

darray * 
huffman_tree_get_code_of_letter
(
    darray_iterator it
)
{
    stack *reverse_code = stack_create(sizeof(bool));

    huffman_tree_node *node;
    node = darray_iterator_get_value(it, huffman_tree_node *);
    while(node->parent)
    {
        if (node->parent->right_child == node) 
        {
            bool bit = 1;
            stack_push(reverse_code, bit);
        }
        else
        {
            bool bit = 0;
            stack_push(reverse_code, bit);
        }

        node = node->parent;
    }

    darray *code = darray_create(sizeof(bool));
    darray_reserve(code, stack_size(reverse_code));

    while (!stack_empty(reverse_code))
    {
        bool top = stack_top(reverse_code, bool);
        darray_append(code, top);
        stack_pop(reverse_code);
    }

    stack_free(reverse_code);
    return code;
}

darray_iterator
huffman_tree_get_parent_by_son
(
    darray_iterator son
)
{
    huffman_tree_node *parent_node = 
        darray_iterator_get_value(son, huffman_tree_node *)->parent;

    darray_iterator may_be_parent = son;
    while (darray_iterator_get_value(may_be_parent, huffman_tree_node *) != parent_node) {
        darray_iterator_advance(may_be_parent, 1, left);
    }
    return may_be_parent;
}

void
huffman_tree_swap_two_nodes
(
    huffman_tree    *tree,
    darray_iterator  it1,
    darray_iterator  it2
)
{
    huffman_tree_node *replacement_parent = darray_iterator_get_value(it1, huffman_tree_node *)->parent;
    huffman_tree_node *start_el_parent = darray_iterator_get_value(it2, huffman_tree_node *)->parent;

    if (replacement_parent->right_child == darray_iterator_get_value(it1, huffman_tree_node *)) {
        replacement_parent->right_child = darray_iterator_get_value(it2, huffman_tree_node *);
    }
    else {
        replacement_parent->left_child = darray_iterator_get_value(it2, huffman_tree_node *);
    }

    if (start_el_parent->right_child == darray_iterator_get_value(it2, huffman_tree_node *)) {
        start_el_parent->right_child = darray_iterator_get_value(it1, huffman_tree_node *);
    }
    else {
        start_el_parent->left_child = darray_iterator_get_value(it1, huffman_tree_node *);
    }

    darray_iterator_get_value(it1, huffman_tree_node *)->parent = start_el_parent;
    darray_iterator_get_value(it2, huffman_tree_node *)->parent = replacement_parent;

    darray_iterator_swap(it1, it2); 
}

darray_iterator
huffman_tree_looking_for_replacement
(
    huffman_tree    *tree,  
    darray_iterator  start_el
)
{
    darray_iterator curr_el;
    curr_el = start_el;
    darray_iterator_advance(curr_el, 1, left);

    huffman_tree_node *start_el_value, *curr_el_value;
    start_el_value = darray_iterator_get_value(start_el, huffman_tree_node *);
    curr_el_value = darray_iterator_get_value(curr_el, huffman_tree_node *);

    while ((darray_iterator_compare(curr_el, darray_begin(tree->nodes)) != 0)
        && (curr_el_value->frequency < start_el_value->frequency))
    {
        darray_iterator_advance(curr_el, 1, left);
        curr_el_value = darray_iterator_get_value(curr_el, huffman_tree_node *);
    }

    darray_iterator_advance(curr_el, 1, right);
    return curr_el;
}

void
huffman_tree_restore_properties_of_tree
(
    huffman_tree    *tree,
    darray_iterator  start_el
)
{
    while (darray_iterator_compare(start_el, darray_begin(tree->nodes)) != 0)
    {
        darray_iterator_get_value(start_el, huffman_tree_node *)->frequency++;

        darray_iterator may_be_replacement = 
            huffman_tree_looking_for_replacement(tree, start_el);
        if (darray_iterator_compare(may_be_replacement, start_el) != 0)
        /* Замена была найдена, значит, необходимо менять узлы местами */
        {
            huffman_tree_swap_two_nodes(tree, may_be_replacement, start_el);
            start_el = huffman_tree_get_parent_by_son(may_be_replacement);
        }
        else
        {
            start_el = huffman_tree_get_parent_by_son(start_el);
        }
    }

    darray_iterator_get_value(start_el, huffman_tree_node *)->frequency++;   
}

darray *
huffman_tree_convert_letter_to_bits
(
    const uint8_t letter
)
{
    darray *bits = darray_create(sizeof(bool));
    darray_reserve(bits, CHAR_BIT);

    for (int i = CHAR_BIT - 1; i >= 0; --i) 
    {
        bool bit;
        bit = (letter >> i) & 1;
        darray_append(bits, bit);
    }

    return bits;
}

uint8_t
huffman_tree_convert_bits_to_letter
(
    const darray *bits
)
{
    uint8_t letter = 0;

    for (size_t i = 0; i < CHAR_BIT; ++i)
    {
        if (darray_at((darray *)bits, i, bool)) {
            letter += (int)pow(2, CHAR_BIT - 1 - i);
        }
    }

    return letter;
}

darray *
huffman_tree_insert
(
    huffman_tree *tree,
    uint8_t          letter
)
{
    darray *bits;

    huffman_tree_node *new_node = huffman_tree_node_create(); 
    new_node->letter = letter;
    new_node->is_fictitious = false;

    darray_iterator find_res = darray_find(tree->nodes, new_node, huffman_tree_node_compare);

    if (darray_iterator_compare(find_res, darray_end(tree->nodes)) != 0)
    {
        free(new_node);
        bits = huffman_tree_get_code_of_letter(find_res);
        huffman_tree_restore_properties_of_tree(tree, find_res);
    }
    else
    {
        darray_iterator art_node = find_res;
        darray_iterator_advance(art_node, 1, left);

        bits = huffman_tree_get_code_of_letter(art_node);
        darray *bits_of_letter = huffman_tree_convert_letter_to_bits(letter);

        // TODO переписать на итераторы
        for
        (
            darray_iterator it = darray_begin(bits_of_letter);
            darray_iterator_compare(it, darray_end(bits_of_letter)) != 0;
            darray_iterator_advance(it, 1, right)
        )
        {
            darray_append(bits, darray_iterator_get_value(it, bool));
        }

        darray_free(bits_of_letter);

        new_node->parent = darray_at(tree->nodes, darray_size(tree->nodes) - 1, huffman_tree_node *);
        new_node->parent->left_child = new_node;
        darray_append(tree->nodes, new_node);

        huffman_tree_node *fict_node = huffman_tree_node_create();
        fict_node->parent = darray_at(tree->nodes, darray_size(tree->nodes) - 2, huffman_tree_node *);
        fict_node->parent->right_child = fict_node;
        darray_append(tree->nodes, fict_node);

        darray_iterator beggining_of_ascent = darray_end(tree->nodes);
        darray_iterator_advance(beggining_of_ascent, 3, left);

        // for (size_t i = 0; i < darray_size(tree->nodes); ++i)
        // {
        //     huffman_tree_node *node = 
        //         darray_at(tree->nodes, i, huffman_tree_node *);
        //     printf("%lu - %c - %d - %p - %p - %p - %p\n",
        //         node->frequency, node->letter, node->is_fictitious, node, node->parent, node->left_child, node->right_child);
        // }
        // printf("\n\n\n");

        huffman_tree_restore_properties_of_tree(tree, beggining_of_ascent);
    }

    return bits;
}

bitset *
huffman_tree_encode
(
    const darray *bytes
)
{
    huffman_tree *tree = huffman_tree_create();

    bitset *result = bitset_create();

    for (size_t i = 0; i < darray_size((darray *)bytes); ++i)
    {
        darray *code = huffman_tree_insert(tree, darray_at((darray *)bytes, i, uint8_t));

        // TODO переписать на итераторы 
        for
        (
            darray_iterator it = darray_begin(code);
            darray_iterator_compare(it, darray_end(code)) != 0;
            darray_iterator_advance(it, 1, right)
        )
        {
            bool curr_bit = darray_iterator_get_value(it, bool);
            bitset_append(result, curr_bit == true ? true : false);
        }

        darray_free(code);
    }

    huffman_tree_free(tree);
    return result;
}

darray_iterator
huffman_tree_get_son_by_parent
(
    darray_iterator parent,
    const bool            bit
)
{
    huffman_tree_node *son;

    if (bit == 0) {
        son = darray_iterator_get_value(parent, huffman_tree_node *)->left_child;
    }
    else {
        son = darray_iterator_get_value(parent, huffman_tree_node *)->right_child;
    }

    while (darray_iterator_get_value(parent, huffman_tree_node *) != son) {
        darray_iterator_advance(parent, 1, right);
    }
    return parent;
}

darray *
huffman_tree_decode
(
    const bitset *bits
)
{
    huffman_tree *tree = huffman_tree_create();

    darray *result = darray_create(sizeof(uint8_t));

    darray_iterator nodes_it = darray_begin(tree->nodes);

    for (size_t i = 0; i < bitset_size((bitset *)bits); )
    {
        huffman_tree_node *nodes_it_value = darray_iterator_get_value(nodes_it, huffman_tree_node *);
        if (nodes_it_value->left_child == NULL && nodes_it_value->right_child == NULL)
        /* пришли в лист */
        {
            if (nodes_it_value->is_fictitious)
            /* пришли в лист, который содержит искусственный символ */
            {
                darray *code_of_letter = darray_create(sizeof(bool));
                darray_reserve(code_of_letter, CHAR_BIT);

                for (size_t j = 0; j < CHAR_BIT; ++j)
                {
                    bool bit = bitset_at((bitset *)bits, i);
                    darray_append(code_of_letter, bit);
                    ++i;
                }

                uint8_t letter = huffman_tree_convert_bits_to_letter(code_of_letter);
                darray_free(code_of_letter);

                darray_append(result, letter);

                huffman_tree_insert(tree, letter);
            }
            else
            /* пришли в лист, который содержит какой-то байт */
            {
                uint8_t letter = darray_iterator_get_value(nodes_it, huffman_tree_node *)->letter;

                darray_append(result, letter);

                huffman_tree_insert(tree, letter);
            }

            nodes_it = darray_begin(tree->nodes);
        }
        else
        {
            nodes_it = huffman_tree_get_son_by_parent(nodes_it, bitset_at((bitset *)bits, i));
            ++i;
        }
    }

    if (darray_iterator_get_value(nodes_it, huffman_tree_node *)->is_fictitious == false) 
    {
        uint8_t let = darray_iterator_get_value(nodes_it, huffman_tree_node *)->letter;
        darray_append(result, let);
    }

    huffman_tree_free(tree);
    return result;
}
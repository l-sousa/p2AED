#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define InitialSize 1059

typedef struct node
{
    long word_pos;
    long word_num;
    long word_count;
    char word[64];
    long first_pos;
    long current_pos;
    long max_distance;
    long min_distance;
    long med_distance;
    long last_pos;
    long prev_pos;
    FILE *fp;
    struct node *next;
} node_t;
/*----------------------------------------------*/

long htCounter = 1;
node_t *hashTable[InitialSize] = {NULL};
long size = 1059;
/*----------------------------------------------*/

/*Hash Fucntion utilizada para a hash table*/
unsigned int hash_function(const char *str, unsigned int s)
{
    static unsigned int table[256];
    unsigned int crc, i, j;
    if (table[1] == 0u) // do we need to initialize the table[] array?
    {
        for (i = 0u; i < 256u; i++)
            for (table[i] = i, j = 0u; j < 8u; j++)
                if (table[i] & 1u)
                    table[i] = (table[i] >> 1) ^ 0xAED00022u; // "magic" constant
                else
                    table[i] >>= 1;
    }
    crc = 0xAED02019u; // initial value (chosen arbitrarily)
    while (*str != '\0')
        crc = (crc >> 8) ^ table[crc & 0xFFu] ^ ((unsigned int)*str++ << 24);
    return crc % s;
}
/*----------------------------------------------*/

/*Função que dá print da linked list*/
void printHashTable()
{
    for (int i = 0; i < size; i++)
    {
        node_t *current = hashTable[i];
        if (current == NULL)
        {
            continue;
        }
        printf("HashTable Position %d\n", i);

        while (current != NULL)
        {
            printf("%s >>> wCount %ld | FirstPos %ld | LastPos %ld | MaxDist %ld | MinDist %ld | AvgDist %ld\n", current->word, current->word_count, current->first_pos, current->last_pos, current->max_distance, current->min_distance, current->med_distance);
            current = current->next;
        }

        printf("\n");
    }
}

/*----------------------------------------------*/
int open_text_file(char *file_name, node_t *fd)
{
    fd->fp = fopen(file_name, "rb");
    if (fd->fp == NULL)
    {
        printf("Invalid text file\n");
        return -1;
    }
    fd->word_pos = -1;
    fd->word_num = -1;
    fd->word[0] = '\0';
    fd->current_pos = -1;
    return 0;
}

void close_text_file(node_t *fd)
{
    fclose(fd->fp);
    fd->fp = NULL;
}

int read_word(node_t *fd)
{
    int i, c;
    // skip white spaces
    do
    {
        c = fgetc(fd->fp);
        if (c == EOF)
            return -1;
        fd->current_pos++;
    } while (c <= 32);
    // record word
    fd->word_pos = fd->current_pos;
    fd->word_num++;
    fd->word[0] = (char)c;
    for (i = 1; i < (int)sizeof(fd->word) - 1; i++)
    {
        c = fgetc(fd->fp);
        if (c == EOF)
            break; // end of file
        fd->current_pos++;
        if (c <= 32)
            break; // terminate word
        fd->word[i] = (char)c;
    }
    fd->word[i] = '\0';
    return 0;
}
/*Insere elemento na hash Table*/
void insert(node_t *word_info)
{
    long current_distance;
    int hcode = hash_function(word_info->word, size);

    if (hashTable[hcode] == NULL)
    {
        htCounter++;
        hashTable[hcode] = word_info;
        hashTable[hcode]->first_pos = word_info->current_pos - strlen(word_info->word);
        hashTable[hcode]->last_pos = hashTable[hcode]->first_pos;
        hashTable[hcode]->word_count = 1;
        hashTable[hcode]->max_distance = 0;
        hashTable[hcode]->min_distance = 0;
        hashTable[hcode]->med_distance = 0;
        hashTable[hcode]->next = NULL;
        hashTable[hcode]->prev_pos = word_info->current_pos;
        return;
    }

    node_t *current = hashTable[hcode];
    while (current != NULL)
    {
	//Se houver colisão mas
		if (&current->word_count == NULL)
        {
			htCounter++;
       		hashTable[hcode] = word_info;
       		hashTable[hcode]->first_pos = word_info->current_pos;
      		hashTable[hcode]->last_pos = hashTable[hcode]->first_pos;
     		hashTable[hcode]->word_count = 1;
      		hashTable[hcode]->max_distance = 0;
    		hashTable[hcode]->min_distance = 0;
     		hashTable[hcode]->med_distance = 0;
      		hashTable[hcode]->next = NULL;
     		hashTable[hcode]->prev_pos = word_info->current_pos;
        	return;
        }
        // se as palavras forem as mesmas (e hashcode igual)
        // neste caso apenas temos de atualizar as informacoes do node
        if (strcmp(current->word, word_info->word) == 0)
        {	
			// calcula a distancia atual entre duas palavras (palavra nova - palavra antiga)
            current_distance = word_info->word_pos - current->word_pos;
			if(current->word_count == 1)
			{
				current->min_distance=current_distance;
			}
            current->word_count++;
            current->last_pos = word_info->current_pos;
            current->max_distance = current->last_pos - current->first_pos;
            // atualiza a posição global da ultima palavra
            current->word_num = word_info->word_num;
            current->word_pos = word_info->word_pos;
            if (current_distance < current->min_distance)
            {
                current->min_distance = current_distance;
            }
            if (current_distance > current->max_distance)
            {
                current->max_distance = current_distance;
            }
            current->current_pos = word_info->current_pos;
            current->med_distance = (current->med_distance + (current->current_pos - current->prev_pos)) / (current->word_count - 1);
            break;
        }
        current = current->next;
    }
}
/*----------------------------------------------*/
node_t *new_node(node_t *fnode)
{
    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    strcpy(new_node->word, fnode->word);
    new_node->word_pos = fnode->word_pos;
    new_node->word_num = fnode->word_num;
    new_node->first_pos = fnode->first_pos;
    new_node->last_pos = fnode->last_pos;
    new_node->current_pos = fnode->current_pos;
    new_node->max_distance = fnode->max_distance;
    new_node->min_distance = fnode->min_distance;
    new_node->med_distance = fnode->med_distance;
    return new_node;
}
void resize()
{
    printf("REASHING:::::::::::::::::::\n");
    int new_size = size * 2;
    int k = 0;
    node_t *newHT[new_size];
    node_t *next = malloc(sizeof(node_t));
    node_t *curr = malloc(sizeof(node_t));

    for (int i = 0; i < new_size; i++)
    {
        newHT[i] = NULL;
    }

    for (int i = 0; i < size; i++)
    {
        next = hashTable[i];
        hashTable[i] = NULL;
        while (next)
        {
            curr = next;
            next = next->next;

            k = hash_function(curr->word, size) % new_size;

            if (curr != newHT[k])
            {
                curr->next = newHT[k];
                newHT[k] = curr;
            }
        }
    }
    *hashTable = *newHT;
}
int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        printf("Invalid Number of Arguments\n");
        return -1;
    }

    node_t *nd = malloc(sizeof(node_t));
    char fname[32];
    strcpy(fname, argv[1]);
    if (open_text_file(fname, nd) == -1)
    {
        return -1;
    }

    while (read_word(nd) == 0)
    {
        node_t *node = new_node(nd);
        if (htCounter >= size / 2)
        {
            resize();
        }
        insert(node);
    }
    close_text_file(nd);
    printHashTable();
    return 0;
}

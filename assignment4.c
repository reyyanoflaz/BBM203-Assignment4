/*
 ============================================================================
 Name        : assignment4.c
 Author      : Reyyan Oflaz
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define alpha_size 26
#define ARRAY_SIZE(a) sizeof(a)/sizeof(a[0])

// Alphabet size (# of symbols)
#define ALPHABET_SIZE (26)

// Converts key current character into index
// use only 'a' through 'z' and lower case
#define CHAR_TO_INDEX(c) ((int)c - (int)'a')

typedef enum enum_trie_t{
	ENUM_TRIE_NOT_END_OF_WORD,
	ENUM_TRIE_END_OF_WORD
}enum_trie;

typedef enum enum_trie_reserve_t{
	ENUM_TRIE_USERNAME_RESERVED,
	ENUM_TRIE_USERNAME_NOT_RESERVED
}trie_reserve;
typedef struct trie_node_t{
    struct trie_node_t *children[ALPHABET_SIZE];
    enum_trie e_trie_node;
    int password;
    // isEndOfWord is true if the node represents
    // end of a word

   // bool isEndOfWord;
}trie_node;


typedef struct command_parameters_t{
	char command_parameters[3][30];
	char *command_parameters_alt;
}command_parameters;

void trie_insert(trie_node **root, const char *key);
bool trie_search(trie_node *root, const char *key);
trie_node *trie_get_node(void);
trie_node* trie_remove(trie_node *root, char *key, int depth);
bool trie_is_empty(trie_node *root);
void readFile(FILE **file,char *fileName,const char *mode);
void parse_exec_command(FILE *filePtr,char *line, size_t line_size,trie_node **root,command_parameters *command_params);
int parse_add_command(char *command_line,command_parameters *command_params);
void execute_add_command(trie_node **root,char *key,int password);
void exec_search_command(char *command_line,command_parameters *command_params);
void exec_query_command(char *command_line,command_parameters *command_params);
void exec_delete_command(char *command_line, command_parameters *command_params);
void exec_list_command(char *command_line,command_parameters *command_params);
void tokenize_line( char param[][30],char *current_line);
int stringToInteger(char *current_line);
int main(int argc, char **argv) {

	// Input keys (use only 'a' through 'z' and lower case)
	char keys[][10] = { "the", "a", "there",
            "answer", "any", "by",
            "bye", "their", "hero", "heroplane" };

	char output[][32] = {"Not present in trie", "Present in trie"};
	FILE *input_file_ptr;
	const size_t line_size = 300;
	char line[300];
	trie_node *root = trie_get_node();
	readFile(&input_file_ptr,argv[1],"r");
	command_parameters command_params;
	parse_exec_command(input_file_ptr,line,line_size,&root,&command_params);

	// Construct trie
	int i;
	for (i = 0; i < ARRAY_SIZE(keys); i++)
		trie_insert(root, keys[i]);

	// Search for different keys
	printf("%s --- %s\n", "the", output[trie_search(root, "the")] );
	printf("%s --- %s\n", "these", output[trie_search(root, "these")] );
	printf("%s --- %s\n", "their", output[trie_search(root, "their")] );
	printf("%s --- %s\n", "thaw", output[trie_search(root, "thaw")] );

	trie_remove(root,"heroplane",0);
	printf("%s --- %s\n", "hero", output[trie_search(root, "hero")] );
	fclose(input_file_ptr);
}


// If not present, inserts key into trie
// If the key is prefix of trie node, just marks leaf node
void trie_insert(trie_node **root, const char *key)
{
    int level;
    int length = strlen(key);
    int index;

    trie_node *p_crawl = (trie_node*)(*root);

    for (level = 0; level < length; level++)
    {
        index = CHAR_TO_INDEX(key[level]);
        if (!p_crawl->children[index]){
        	p_crawl->children[index] = trie_get_node();
        }

        p_crawl = p_crawl->children[index];				// CASE 2-first n character of the username exists on the tree,
    }

    // mark last node as leaf
    p_crawl->e_trie_node = ENUM_TRIE_END_OF_WORD;
}

/*
 *@brief  Returns true if key presents in trie, else false
 */
bool trie_search(trie_node *root, const char *key)
{
    int level;
    int length = strlen(key);
    int index;
    trie_node *pCrawl = root;
    for (level = 0; level < length; level++)
    {
        index = CHAR_TO_INDEX(key[level]);
        if (!pCrawl->children[index]){
            return false;
        }
        pCrawl = pCrawl->children[index];
    }
    return (pCrawl != NULL && (pCrawl->e_trie_node==ENUM_TRIE_END_OF_WORD));
}

/*
 *	@brief Returns new trie node (initialized to NULLs)
 */
trie_node *trie_get_node(void)
{
    trie_node *p_node = NULL;
    p_node = ( trie_node *)malloc(sizeof( trie_node ));
    if (p_node)
    {
        int i;
        p_node->e_trie_node = ENUM_TRIE_NOT_END_OF_WORD;
        for (i = 0; i < ALPHABET_SIZE; i++){
            p_node->children[i] = NULL;
        }
    }
    return p_node;
}
/*
 *	@brief Returns true if root has no children, else false
 *
 */
bool trie_is_empty(trie_node* root)
{
    for (int i = 0; i < ALPHABET_SIZE; i++){
        if (root->children[i]){
            return false;
        }
    }
    return true;
}

// Recursive function to delete a key from given Trie
trie_node* trie_remove(trie_node *root, char *key, int depth)
{
    // If tree is empty
    if (!root){
        return NULL;
    }
    // If last character of key is being processed
    if (depth ==strlen(key)) {
        // This node is no more end of word after removal of given key
        if (root->e_trie_node==ENUM_TRIE_END_OF_WORD){
            root->e_trie_node = ENUM_TRIE_NOT_END_OF_WORD;
        }
        // If given is not prefix of any other word
        if (trie_is_empty(root)) {
        	free(root);
        	//delete (root);
            root = NULL;
        }
        return root;
    }
    // If not last character, recur for the child obtained using ASCII value
    int index = key[depth] - 'a';
    root->children[index] = trie_remove(root->children[index], key, depth + 1);
    // If root does not have any child (its only child got deleted), and it is not end of another word.
    if (trie_is_empty(root) && root->e_trie_node == ENUM_TRIE_NOT_END_OF_WORD) {
    	free(root);
        //delete (root);
        root = NULL;
    }
    return root;
}
/*
 * @brief   Function to check if current node is leaf node or not
 */
bool isLeafNode(trie_node* root)
{
    return root->e_trie_node==ENUM_TRIE_END_OF_WORD;
}


// function to display the content of Trie
void display(trie_node* root, char str[], int level)
{
    // If node is leaf node, it indicates end of string, so a null character is added
    // and string is displayed
    if (isLeafNode(root))
    {
        str[level] = '\0';
        printf("display");
     //   cout << str << endl;
    }
    int i;
    for (i = 0; i < alpha_size; i++)
    {
        // if NON NULL child is found add parent key to str and
        // call the display function recursively for child node
        if (root->children[i])
        {
            str[level] = i + 'a';
            display(root->children[i], str, level + 1);
        }
    }
}


void readFile(FILE **file,char *fileName,const char *mode){

	if(((*file) = fopen(fileName, mode)) == NULL){
		perror("Cannot open input file\n");
		exit(-1);
	}
}
/*
 * -a username password #add username to the tree with the given password
 * -s username #search with the given username and return the password if it is
 * -q username password #send query for the password with the given username
 * -d username #delete username and its password
 * -l #list the tree
 */
void parse_exec_command(FILE *filePtr,char *line, size_t line_size,trie_node **root,command_parameters *command_params){
	while(fgets(line,line_size,filePtr)){
		if(line[strlen(line)-1] == '\n'){
			line[strlen(line)-1] = '\0';
		}
		if(strstr(line,"-a")!= NULL){
			int password;
			password=parse_add_command(line,command_params);

		}
		else if(strstr(line,"-s")){
			exec_search_command(line,command_params);
		}
		else if(strstr(line,"-q")){
			exec_query_command(line,command_params);
		}
		else if(strstr(line,"-d")){
			exec_delete_command(line,command_params);
		}
		else if(strstr(line,"-l")){
			exec_list_command(line,command_params);
		}
	}
}
int parse_add_command(char *command_line,command_parameters *command_params){
	printf("%s\n",command_line);
	int password;
	tokenize_line(command_params->command_parameters,command_line);
	printf("param 1: %s \n",&command_params->command_parameters[1][0]);
	password=stringToInteger(&command_params->command_parameters[2][0]);
	return password;
}
void execute_add_command(trie_node **root,char *key,int password){

	//CASE 1 :reserved key

	//CASE 2: non reserved key

}
void exec_search_command(char *command_line,command_parameters *command_params){
	printf("%s\n",command_line);
}
void exec_query_command(char *command_line,command_parameters *command_params){
	printf("%s\n",command_line);
}
void exec_delete_command(char *command_line,command_parameters *command_params){
	printf("%s\n",command_line);
}
void exec_list_command(char *command_line,command_parameters *command_params){
	printf("%s\n",command_line);
}
void tokenize_line( char param[][30],char *current_line){
    char *split;
    int i=0;
    split= strtok (current_line," ");
    size_t copy_size;
    while(split != NULL) {
    	copy_size=strlen(split);
    	memcpy(&param[i][0], split, (copy_size+1)*sizeof(char));
        split = strtok(NULL, " ");
        i++;
	}
}
int stringToInteger(char *current_line){
	long keyLong;
	char *p;
	int client_number_of;
	const size_t copy_size = 10;
	keyLong=strtol(current_line,&p,10);
    client_number_of = keyLong;
	/*
	if (errno != 0 || *p != '\0' || keyLong > INT_MAX) {
		printf("Error: not valid argument\n");
	} else {
		client_number_of = keyLong;
	}
	*/
	return client_number_of;
}

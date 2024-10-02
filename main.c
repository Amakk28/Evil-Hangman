#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

typedef struct Pattern_struct {
    char* pat;  //string pattern - exs: "hello", "-ell-", "-----"
    int count;  //frequency of pattern in the current word list
    int changes;//number of occurences of new letter in this pattern
} Pattern;

int strDiffInd(char* str1, char* str2);
int strNumMods(char* str1, char* str2);
// function to remove all patterns from pattern array except for chosen one
void remove_patterns(Pattern*** patterns, int* pattern_num, int* final_pattern, int* cap) {
    int deleted = 0;
    for (int i = 0; i < *pattern_num; i++) { // loop through pattern array to delete any pattern that are not the final pattern
        if (i != *final_pattern) {
            free((*patterns)[i]->pat);
            free((*patterns)[i]);
            deleted++;
        }
    }
    *pattern_num -= deleted;
    Pattern** new_patterns = malloc(sizeof(Pattern*) * (*cap)); // allocate size for a new pattern array
    *new_patterns = (*patterns)[*final_pattern];  // add the chosen pattern to that array
    free(*patterns); 
    (*patterns) = new_patterns; // rewire original web to new patterns which is resized
    *final_pattern = 0;
}
//function to add letter to guessed list
void add_guess(char** guessedletters, int* letters_guessed, char letter) {
    char* new_g_letters = malloc(sizeof(char) * (*letters_guessed + 1));// make space for new guessed array
    for (int i = 0; i < *letters_guessed; i++) { // copy letters into new array
        new_g_letters[i] = (*guessedletters)[i];
    }
    new_g_letters[*letters_guessed] = letter; // add in new letter to guessed letter array
    *letters_guessed += 1;
    free(*guessedletters); // free and rewire the original array
    (*guessedletters) = new_g_letters;
}
// function to delete wor from wordlist
void deleteWord(char**** words, int** wordNum, char* wordToDel, int index) {
    for (int i = 0; i < **wordNum; i++) {
        if (strcmp((*(*words))[i], wordToDel) == 0) { // find and delete given word in word list array
            free((*(*words))[i]);
            **wordNum -= 1;
            break;
        }
    }
    char** new_words = (char**)malloc(sizeof(char*) * (**wordNum));
    for (int i = 0; i < **wordNum; i++) { // loop through new old words
        if (i >= index) { // if index is reached, then start to skip extinct index when reassigning
            new_words[i] = (*(*words))[i+1];
        } else { // else add to web normally;
            new_words[i] = (*(*words))[i];
        }
    }
    free((*(*words)));
    (*(*words)) = new_words; // rewire original web to new wordlist which is resized
}
int insert_patterns(Pattern*** patterns, int *wordNum, int *cap, char letter, char*** words, int wordsize, int* pattern_num, bool* built, char** guessedletters, int* guesses, int* correct, char* f_pattern) {
    char temp[wordsize + 1]; // temp build pattern into
    int size_of_cont = 0; // size of pattern array
    char p_container[(*wordNum)][wordsize + 1]; // temp container for patterns
    int change = 0; // temp counter for changes
    bool idi_found = false; // check if identical pattern found
    for (int i = 0; i < *wordNum; i++) {
        for (int j = 0; j <= wordsize; j++) { // loop through wordlist to build pattern
            if (j == wordsize) {
                temp[j] = '\0';
            } else {
                temp[j] = '-';
            }
            for (int k = 0; k < *guesses; k++) {
                if ((*words)[i][j] == (*guessedletters)[k]) { //given all the guessed correct letters, put into pattern
                    temp[j] = (*guessedletters)[k];
                    if ((*guessedletters)[k] == letter) { // and update change if encountering new correct letter
                        change += 1;
                    }
                    break;
                }
            }
        }
        for (int j = 0; j < size_of_cont; j++) { // loop through conatiner of patterns to see how common a pattern is and add to its subitem 
            if (strDiffInd(p_container[j], temp) == strlen(p_container[j])) {
                idi_found = true; // mark as duplicate if identical pattern found
                (*patterns)[j]->count += 1;
            }
        }
        if (idi_found) { // continue looping through words if identical matching pattern found
            idi_found = false;
            change = 0;
            continue;
        }
        strcpy(p_container[size_of_cont], temp); // copy curr pat into temp pattern container
        char* pat = malloc(sizeof(char) * wordsize + 1); // make space for new pattern
        strcpy(pat, temp);
        
        Pattern* new_pattern = malloc(sizeof(Pattern)); // allocate space for new pattern object
        new_pattern->pat = pat; // initialize all member variables for struct
        new_pattern->changes = change;
        new_pattern->count = 1;
        if ((*pattern_num) >= (*cap)) { // if pattern array is too small enlarge and insert
            (*cap) *= 2;
            Pattern** new_pattern_arr = malloc(sizeof(Pattern*) * (*cap)); // make space for new pattern array
            for (int k = 0; k < *pattern_num; k++) { // copy all pattern objects into new pattern array
                new_pattern_arr[k] = (*patterns)[k];
            }
            new_pattern_arr[*pattern_num] = new_pattern; //add in latest new patern
            free((*patterns));
            (*patterns) = new_pattern_arr; // rewire old patterns to new pattern array
            (*pattern_num) += 1;
        } else {
            (*patterns)[*pattern_num] = new_pattern;
            (*pattern_num) += 1;
        }
        size_of_cont += 1; //incrememnt current pat array size
        change = 0;
    }
    int max_count = 0; 
    int chose_p_ind = 0; // final index chosen after going through tie brakers
    for (int i = 0; i < size_of_cont; i++) { // loop through patterns for most common pattern
        if ((*patterns)[i]->count > max_count) {
            max_count = (*patterns)[i]->count;
            chose_p_ind = i;
        }
    }
    int min_change = 200000;
    for (int i = 0; i < size_of_cont; i++) { //loop through patterns for same count typebreaker
        if ((*patterns)[i]->count == max_count) { //if tie for count
            if ((*patterns)[i]->changes <= min_change) { //find minimum change and get pattern index at that
                min_change = (*patterns)[i]->changes;
                chose_p_ind = i;
            }
        }
    }
    int earliest_ind = 100;
    for (int i = 0; i < size_of_cont; i++) { //loop through patterns for same count & changes typebreaker
        if ((*patterns)[i]->count == max_count) { //if tie for count 
            if ((*patterns)[i]->changes == min_change) { //if tie for change
                for (int j = 0; j < wordsize; j++) {
                    if ((*patterns)[i]->pat[j] == letter) {
                        if (j < earliest_ind) {
                            earliest_ind = j;
                            chose_p_ind = i;
                        }
                    }
                }
            }
        }
    }
    int word_l_count = 0; // variables to keep track of correct letters found in word
    int wlcount2 = 0;
    int right_letters = 0;
    int right_w = 0;
    if (strcmp(f_pattern, (*patterns)[chose_p_ind]->pat) == 0) { // if previous pattern is the same as next chosen pattern than correct turned off
        *correct = 0;
    }
    for (int i = 0; i < *wordNum; i++) {//loop through wordlist and first check if 
        if (*correct < 1) {
            for (int j = 0; j < wordsize; j++) { // loop through letters
                for (int k = 0; k < *guesses; k++) { // loop through guessed letters
                    if ((*words)[i][j] == (*guessedletters)[k]) { // get how many correct letters in curr word
                        word_l_count += 1;
                    
                    }
                    if ((*patterns)[chose_p_ind]->pat[j] == (*guessedletters)[k]) {
                        right_letters += 1; // get how many correct letters in chosen pattern
                    }
                }
            }
            if (strNumMods((*words)[i], (*patterns)[chose_p_ind]->pat) == wordsize - word_l_count) {
                wlcount2 = 0; //calculate if the number of differences in between word and pattern match up 
                word_l_count = 0;
                if (strcmp(f_pattern, (*patterns)[chose_p_ind]->pat) != 0) { // if previous patterna and chosen pattern do not match
                    for (int iter = 0; iter < wordsize; iter++) { // filter out words that do not exactly match correct letters by index
                        if ((*words)[i][iter] == (*patterns)[chose_p_ind]->pat[iter]) {
                            right_w += 1;
                        }
                    }
                    if (right_w != right_letters) { // iff letters do not match by index, then delete the word
                        right_letters = 0;
                        right_w = 0;
                        deleteWord(&words, &wordNum, (*words)[i], i);
                        i -= 1;
                        continue;
                    }
                }
                right_letters = 0; // reset variables to avoid improper counting
                right_w = 0;
                continue;
            }
            right_letters = 0;
            right_w = 0;
        }
        if (*correct > 0) { // if previous guess was not right, go through words to check index by index if word and pattern are identical
            for (int j = 0; j < wordsize; j++) {
                if ((*words)[i][j] == (*patterns)[chose_p_ind]->pat[j]) {
                    wlcount2 += 1;
                } else if ((*words)[i][j] == letter){
                    word_l_count += 1;
                }
            }
            if (wlcount2 > (*patterns)[chose_p_ind]->changes && (*patterns)[chose_p_ind]->changes != word_l_count) {
                wlcount2 = 0; // if word contains the letters of the pattern at the exact indeces, then pass
                word_l_count = 0;
                continue;
            } 
        }
        deleteWord(&words, &wordNum, (*words)[i], i); // if previous checks failed, delete word
        i -= 1;
        word_l_count = 0;
        wlcount2 = 0;
    }
    if (strcmp(f_pattern, (*patterns)[chose_p_ind]->pat) == 0) { // if guess is incorrect, set correct to false and true the other way around
        *correct = 0;
    } else if (*built == true){
        *correct = 1;
    }
    *built = true; // turn built on if first pattern array was initialized in function
    return chose_p_ind;
}
// function to add a single word to wordlist and reallocate the list
void addWord(char*** words, int* numWords, int* maxWords, char* newWord) {
    char* curr_word = malloc(sizeof(char) * (strlen(newWord) + 1)); // allocate new word space in heap
    strcpy(curr_word, newWord);
    if ((*numWords) >= (*maxWords)) { // resize array if too small
        (*maxWords) *= 2;
        char** new_words_array =  (char**)malloc(sizeof(char*) * (*maxWords)); // make new array for wordlist
        for (int i = 0; i < *numWords; i++) { // copy wordlist into new array
            new_words_array[i] = (*words)[i];
        }
        new_words_array[*numWords] = curr_word; // add in new word to array
        free((*words)); // free original array
        (*words) = new_words_array; // rewire wordlist to new array
        (*numWords) += 1; 
    } else {
        (*words)[*numWords] = curr_word; // else add new word to latest index
        (*numWords) += 1;
    }
}
// function to return the amount of differences between two characters
int strNumMods(char* str1, char* str2) {
    int size = 0;
    int counter = 0;
    if (strlen(str1) > strlen(str2)) { // get highest length of two str
        size = strlen(str1);
    } else {
        size = strlen(str2);
    }
    for (int i = 0; i < size; i++) { // add to counter if strs encounter difference and return that value
        if (str1[i] == '\0') {
            counter += strlen(str2) - strlen(str1);
            return counter;
        } else if (str1[i] == '\0') {
            counter += strlen(str1) - strlen(str2);
            return counter;
        }
        if (str1[i] != str2[i]) {
            counter += 1;
        }
    }
    return counter;
}
// function to determine the first index where there is a different char between two strings
int strDiffInd(char* str1, char* str2) { 
    int size = 0;
    int counter = 0;
    if (strlen(str1) > strlen(str2)) { //get highest length of two str
        size = strlen(str1);
    } else {
        size = strlen(str2);
    }

  for (int i = 0; i < size; i++) {  // go through strings to return the index that is different
        if (str1[i] != str2[i]) {
            counter = i;
            return counter;
        }
    }
    return size; // or else return the size
}


void sortguesses(char guesses[], int* num_guesses) { // take in the guesses array and number of guesses to sort
    char temp;
    int i, j;
    for (i = 0; i < *num_guesses-1; i++) { // loop through guesses to swap values in ascending order
        for (j = i+1; j < *num_guesses; j++) {
            if (guesses[i] > guesses[j]) {
                temp = guesses[i];
                guesses[i] = guesses[j];
                guesses[j] = temp;
            }
        }
    }
}


int main(int argc, char* argv[]) {
    printf("Welcome to the (Evil) Word Guessing Game!\n\n");
    int wordSize = 5;
    int numGuesses = 26;
    char stats[] = "OFF"; // initialize all variables to false and default values
    char wordListmode[] = "OFF";
    char letterList[] = "OFF";
    char patternList[] = "OFF";
    bool invalid = false;
    bool verbose = false;
    bool solved = false;
    for (int i = 1; i < argc; i++) { // loop through arguements
        if (strchr(argv[i], '-')) { // check the type of arguement by checking for '-' 
            if (strchr(argv[i], 'n')) { //once letters are checked, get next arguement for the number
                wordSize = atoi(argv[i+1]);
            } else if (strchr(argv[i], 'g')) {
                numGuesses = atoi(argv[i+1]);
            } else if (strchr((argv[i]), 's')) {
                strcpy(stats, "ON");
            } else if (strchr((argv[i]), 'w')) { // or if setting is checked, turn that setting on
                strcpy(wordListmode, "ON");
            } else if (strchr((argv[i]), 'l')) {
                strcpy(letterList, "ON");
            } else if (strchr((argv[i]), 'p')) {
                strcpy(patternList, "ON");
            } else if (strchr((argv[i]), 'v')) {
                strcpy(stats, "ON");
                strcpy(wordListmode, "ON");
                strcpy(letterList, "ON");
                strcpy(patternList, "ON");
            } else {
                invalid = true; //if nothing was checked above, then arguement must be invalid type
            }
        }
    }
    if (numGuesses < 1) { // first check if number of guess and word sizes are good
        printf("Invalid number of guesses.\nTerminating program...\n");
        return 0;
    } else if (wordSize < 2 || wordSize > 29) { // check if word size is greater than 29 or less than 2
        printf("Invalid word size.\nTerminating program...\n");
        return 0;
    } else if (invalid) {
        printf("Invalid command-line argument.\nTerminating program...\n");
        return 0;
    } else { // after checking edge case arguements, print out the settings that were enabled
        printf("Game Settings:\n");
        printf("  Word Size = %d\n", wordSize);
        printf("  Number of Guesses = %d\n", numGuesses);
        printf("  View Stats Mode = %s\n", stats);
        printf("  View Word List Mode = %s\n", wordListmode);
        printf("  View Letter List Mode = %s\n", letterList);
        printf("  View Pattern List Mode = %s\n", patternList);
    }
    int capacity = 4;
    char** wordList = (char**)malloc(capacity*sizeof(char*));// initialize word list
    int wordNum = 0;
    int max_length = 0;
    char largest_word[100]; 
    int total_words = 0; //temp variables to keep track of stats
    FILE* dict_file = NULL;
    dict_file = fopen("dictionary.txt", "r");
    char curr_word[100];
    while(!feof(dict_file)) { //open and read from file dictionary 
        total_words += 1;
        fscanf(dict_file, "%s", curr_word);
        if (strlen(curr_word) == wordSize) { // add word to wordlist if size of word matches settings
            addWord(&wordList, &wordNum, &capacity, curr_word);
        }
        if (strlen(curr_word) > max_length) { // determine the max length of a word
            max_length = strlen(curr_word);
            strcpy(largest_word, curr_word);
        }
    }
    fclose(dict_file);
    if (strcmp(stats, "ON") == 0) { // branch to print out dictionary stats
        printf("The dictionary contains %d words total.\n", total_words);
        printf("The longest word %s has %d chars.\n", largest_word, max_length);
        printf("The dictionary contains %d words of length %d.\n", wordNum, wordSize);
        printf("Max size of the dynamic words array is %d.\n", capacity);
    }
    if (strcmp(wordListmode, "ON") == 0) { // branch to turn on wordlist mode
        printf("Words of length %d:\n", wordSize);
        for (int i = 0; i < wordNum; i++) {
            printf("  %s\n", wordList[i]);
        }
    }
    if (wordNum == 0) { // edge case to check if dictionary contains word of given size
        printf("Dictionary has no words of length %d.\n", wordSize);
        printf("Terminating program...\n");
        free(wordList);
        return 0;
    } else { // else print out word pattern
        printf("The word pattern is: "); 
        for (int i = 0; i < wordSize; i++) {
            printf("-");
        }
        printf("\n");
    }

    char input;
    char* guessed_letters = malloc(sizeof(char)); //  guessed letters array
    int letters_guessed = 0;
    bool repeat = false; // sentinal variable to make sure letter isnt repeated
    int cap = 4;
    Pattern** patterns = malloc(sizeof(Pattern*) * cap); // patterns array
    int pattern_num = 0;
    bool built = false; // to check if first patterns array is initialized
    int final_pattern = 0; //index for final chosen pattern
    int correct = 0; // checks if last guess is correct
    char f_pattern[wordSize + 1]; // temp container for final pat
    bool invalid_letter = false;
    for (int i = 0; i < wordSize; i++) { // clean temp pat of garbage values before use
        f_pattern[i] = '\0';
    }
    do {
        if (invalid_letter == false) { // if input marked as invalid
            if (repeat == true) { // if letter was guessed before
                printf("\nLetter previously guessed..."); // then letter was already guessed
                repeat = false;
            } else { // else if guesses are 0, terminate
                if (numGuesses == 0) {
                    printf("\nYou ran out of guesses and did not solve the word.\nThe word is: %s\nGame over.\n", wordList[0]);
                    break;
                } // else print out guesses remaing 
                printf("\nNumber of guesses remaining: %d", numGuesses);
                if (strcmp(letterList, "ON") == 0) { // setting for letter list to print
                    sortguesses(guessed_letters, &letters_guessed); // first sort letters alphabetically
                    printf("\nPreviously guessed letters: ");
                    for (int h = 0; h < letters_guessed; h++) {
                        printf("%c ", guessed_letters[h]);
                    }
                }
            }
        }
        printf("\nGuess a letter (# to end game): \n"); // get letter input from use
        scanf(" %c", &input);
        if (!isalpha(input) || isupper(input)) { // check if input is a letter and not uppercase
            if (input == '#') { // if input is # then terminate
                printf("\nTerminating game...\n");
                break;
            }
            printf("Invalid letter..."); // else mark input as invalid and continue
            invalid_letter = true;
            continue;
        } else {
            invalid_letter = false; 
        }
        for (int i = 0; i < letters_guessed; i++) { // check if input was a letter previously guessesd
            if (guessed_letters[i] == input) {
                repeat = true;
                continue;
            }
        }
        if (!repeat) {  // if cases are valid, add letter to guessed letters array
            add_guess(&guessed_letters, &letters_guessed, input); // insert pattern and get back index to chosen pattern
            final_pattern = insert_patterns(&patterns, &wordNum, &cap, input, &wordList, wordSize, &pattern_num, &built, &guessed_letters, &letters_guessed, &correct, f_pattern);
            if (strcmp(patternList, "ON") == 0) { // setting for listing patterns, and their subitems
                printf("All patterns for letter %c:\n", input);
                for (int i = 0; i < pattern_num; i++) {
                    printf("  %s    count = %-8d  changes = %d\n", patterns[i]->pat, patterns[i]->count, patterns[i]->changes);
                }
            }
            if (strchr(patterns[final_pattern]->pat, input)) { //guess was correct
                printf("Good guess! The word has at least one %c.\n", input);
            } else {
                printf("Oops, there are no %c's. You used a guess.\n", input); // if guess not correct decrement numGuesses
                numGuesses -= 1;
            }
            if (strcmp(stats, "ON") == 0) { // setting for listing words remaining
                printf("Number of possible words remaining: %d\n", wordNum);
            }
            if (strcmp(wordListmode, "ON") == 0) { 
                printf("Possible words are now:\n"); // print out wordlist for setting
                for (int i = 0; i < wordNum; i++) {
                    printf("  %s\n", wordList[i]);
                }
            }
            remove_patterns(&patterns, &pattern_num, &final_pattern, &cap); // remove all but the final pattern in pattern array
            printf("The word pattern is: %s\n", patterns[final_pattern]->pat);
            if (wordNum == 1) { // if only 1 word left in list then keep checking if the final pattern is fully complete
                if (strcmp(wordList[0], patterns[final_pattern]->pat) == 0) {
                    printf("\nYou solved the word!\nThe word is: %s\nGame over.\n", wordList[0]);
                    break;
                }
            }
            strcpy(f_pattern, patterns[final_pattern]->pat); //copy final pattern into temp f_pattern and free the final patterns block
            free(patterns[final_pattern]->pat);
            free(patterns[final_pattern]);
            pattern_num -= 1;
        }
        
    } while((input) != '#'); // if number of guesses runs out or # as input, terminate loop
    

    for (int i = 0; i < wordNum; i++) { // free all allocated dynamic array memory of wordlist, letter guesses, and patterns
        free(wordList[i]);
    }
    free(wordList);
    free(guessed_letters);
    for (int i = 0; i < pattern_num; i++) {
        free(patterns[i]->pat);
    }
    for (int i = 0; i < pattern_num; i++) {
        free(patterns[i]);
    }
    free(patterns);
    return 0;
}

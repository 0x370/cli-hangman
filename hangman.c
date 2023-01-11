#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include "types.h"

/*
    define windows if you're on a windows machine, otherwise feel free to uncomment
    used when cleaning the screen
*/
#define WINDOWS

/*
    max amount of guesses we want the user to have
    if you increase/decrease this remember to update draw_game to reflect on the changes
*/
#define MAX_GUESS 5

/* different game states used in the check_guess function */
#define REPEAT 1337
#define VICTORY 1338

/*
    Draws hangman to the console

    @param wrong_guesses: amount of wrong guesses by the user
    @param guessed: array to read guessed characters from
    @param correct: array to read correctly guessed characters from
    @return nothing
*/
void draw_game(const u32 wrong_guesses, const u8 *guessed, const u8 *correct)
{
#ifdef WINDOWS
        /* using system is bad practice, but it's also the simplest...*/
        system("cls");
#else
        printf("\e[1;1H\e[2J");
#endif // WINDOWS

        printf("guessed: %s\n", guessed);
        printf("correct: %s\n", correct);

        for (u32 i = 1; i <= wrong_guesses; ++i)
        {
                switch (i)
                {
                case 1:
                case 2:
                case 5:
                        printf("-");
                        break;
                case 3:
                        printf("o");
                        break;
                case 4:
                        printf("<");
                        break;
                case 6:
                        printf("<\n\ngame over");
                        break;
                default:
                        break;
                }
        }
}

/*
    Hangman sanity check and game logic

    @param c: character to check
    @param word: word to check against
    @param word_length: length of the word to check against
    @param guessed: array to insert guessed characters
    @param correct: array to insert correctly guessed characters
    @return amount of guesses or MAX_AMOUNT + 1 if failed
*/
u32 check_guess(const u8 c, const u8 *word, const u32 word_length, u8 *guessed, u8 *correct)
{
        if (word == NULL || guessed == NULL || correct == NULL)
                return MAX_GUESS + 1;

        static u32 guessed_count = 0;
        u8 valid = 0;

        /* check for repeated guess */
        for (i32 i = 0; i < guessed_count; ++i)
        {
                if (guessed[i] == c)
                {
                        return REPEAT;
                }
        }

        /* check if it is a valid guess */
        for (i32 i = 0; i < word_length; ++i)
        {
                if (word[i] == c)
                {
                        correct[i] = c;
                        valid = 1;

                        /* check if we won */
                        if (strcmp(word, correct) == 0)
                        {
                                printf("\n\nyou won!");
                                return VICTORY;
                        }
                }
        }

        /* if we ended up here it means it was a valid but incorrect guess */
        guessed[guessed_count] = c;

        if (valid == 0)
                return ++guessed_count;

        return 0;
}

/*
    Get a random line from a file

    @param input_file: file to read from
    @return random line from input_file or NULL if failed
*/
u8 *get_random_word(const u8 *input_file)
{
        FILE *f = NULL;
        u32 line_number = 0;
        u32 line_count = 0;
        u32 current_length = 0;
        u8 current[256] = {[0] = '\0'};

        time_t t;
        srand((unsigned)time(&t));

        f = fopen(input_file, "r");
        if (f == NULL)
        {
                printf("failed to open file %s (%s)\n", input_file, strerror(errno));
                return NULL;
        }

        for (line_count = 0; fgets(current, sizeof(current), f); ++line_count)
                ;

        if (line_count == 0)
        {
                printf("file is empty!");
                return NULL;
        }

        line_number = rand() % (line_count + 1);
        rewind(f);

        for (u32 i = 0; i < line_number; ++i)
        {
                fgets(current, sizeof(current), f);
        }

        fclose(f);

        current_length = strlen(current);
        if (current_length > 0 && current[current_length - 1] == '\n')
        {
                current[current_length - 1] = '\0';
        }

        return strdup(current);
}

i32 main(i32 argc, u8 **argv)
{
        u8 guessed_word[256] = {0};
        u8 correct_word[256] = {0};
        u32 current_stage = 0;

        const u8 *word = get_random_word("words.txt");
        if (word == NULL)
                return EXIT_FAILURE;

        const u32 word_length = strlen(word);

        for (u32 i = 0; i < word_length; ++i)
        {
                correct_word[i] = '_';
        }

        while (current_stage <= MAX_GUESS)
        {
                draw_game(current_stage, guessed_word, correct_word);
                printf("\nenter guess: ");

                u8 guess = 0;
                i32 temp = scanf(" %c", &guess);

                u32 result = check_guess(guess, word, word_length, guessed_word, correct_word);

                if (result == REPEAT)
                        continue;

                if (result == VICTORY)
                        break;

                if (result > 0)
                        current_stage = result;
        }
        _sleep(5000);

        if (word != NULL)
                free(word);

        return EXIT_SUCCESS;
}

# Evil-Hangman
Overview
Evil Hangman is a twist on the classic Hangman game where the computer cheats to make the game more challenging for the player. Instead of selecting a single word at the start, the computer dynamically changes the word to dodge the player’s guesses, making it difficult to guess the correct word.

How It Works
Word List: The game starts with a list of all possible words of a given length.
Player Guesses: The player guesses one letter at a time.
Word Families: After each guess, the computer partitions the word list into “word families” based on the positions of the guessed letter.
Choosing the Largest Family: The computer then selects the largest word family, effectively avoiding the guessed letter as much as possible.
Updating the Display: The game updates the displayed word with the guessed letters if they are present in the chosen word family.
Game Continuation: The process repeats until the player either guesses the word or runs out of guesses.

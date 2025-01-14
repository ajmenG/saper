// Główny plik, parsuje argumenty, uruchamia grę
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include "../include/board.h"
#include "../include/game.h"
#include "../include/parse_file.h"
#include <bits/getopt_core.h>

extern int revealed;

int run = 1;

int fail = 0;

void interactive_mode();

void file_mode();

void print_usage(char *name);

int main(int argc, char *argv[])
{
    int opt;
    char *filename = NULL;
    int flag_found = 0;

    while ((opt = getopt(argc, argv, "f:")) != -1)
    {
        switch (opt)
        {
        case 'f':
            filename = optarg;
            printf("Nazwa pliku: %s\n", filename);
            file_mode(filename);
            flag_found = 1;
            break;
        default:
            print_usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    if (!flag_found)
    {
        interactive_mode();
    }

    return 0;
}

void interactive_mode()
{
    rules();

    int rows, cols, difficulty, mines;
    printf("Podaj poziom trudności (1-3) lub 4 (własny): ");
    scanf("%d", &difficulty);

    switch (difficulty)
    {
    case 1:
        rows = 9;
        cols = 9;
        mines = 10;
        break;
    case 2:
        rows = 16;
        cols = 16;
        mines = 40;
        break;
    case 3:
        rows = 16;
        cols = 30;
        mines = 99;
        break;
    case 4:
        printf("Podaj liczbę wierszy: ");
        scanf("%d", &rows);
        printf("Podaj liczbę kolumn: ");
        scanf("%d", &cols);
        printf("Podaj liczbę min: ");
        scanf("%d", &mines);
        break;
    }

    if (difficulty < 1 || difficulty > 4)
    {
        printf("Niepoprawny poziom trudności\n");
        return EXIT_FAILURE;
    }

    Board *board = board_create(rows, cols, difficulty, mines);
    print_board(board);

    int licznik = 0;

    while (run)
    {
        char command;
        int x, y;

        printf("Podaj komendę: ");
        if (scanf(" %c", &command) != 1) // Sprawdzenie, czy wczytano komendę
        {
            printf("Niepoprawna komenda\n");
            while (getchar() != '\n')
                ; // Wyczyść bufor wejściowy
            continue;
        }

        if (command == 'x') // Sprawdzenie komendy 'x' przed wczytaniem współrzędnych
        {
            fail = 1;
            run = 0;
            break;
        }

        if (command == 'f' || command == 'd' || command == 'r')
        {
            if (scanf("%d %d", &x, &y) != 2) // Sprawdzenie, czy wczytano dwie współrzędne
            {
                printf("Brak współrzędnych\n");
                while (getchar() != '\n')
                    ; // Wyczyść bufor wejściowy
                continue;
            }

            if (x < 1 || x > rows || y < 1 || y > cols)
            {
                printf("Niepoprawne współrzędne\n");
                continue;
            }

            if (licznik == 0)
            {
                generate_mines(board, x - 1, y - 1);
                calculate_mines_in_neighborhood(board);
            }
            licznik++;
        }
        else
        {
            printf("Niepoprawna komenda\n");
            continue;
        }

        switch (command)
        {
        case 'f':
            flag_field(board, x - 1, y - 1);
            break;
        case 'd':
            remove_flag(board, x - 1, y - 1);
            break;
        case 'r':
            reveal_field(board, x - 1, y - 1);
            break;
        }

        printf("\n");
        print_board(board);
        if (count_revealed(board) == board->rows * board->cols - board->mine_count)
        {
            run = 0;
        }
        printf("Aktualny wynik: %d\n", score(board, difficulty));
    }

    if (fail)
    {
        printf("Przegrałeś!!!!!\n");
    }
    else
    {
        printf("Wygrałeś!!!, gratulacje\n");
    }

    printf("Twój wynik to: %d\n\n", score(board, difficulty));

    char odp;
    char nickname[20];
    printf("Czy chcesz zapisać wynik? (t/n): ");
    scanf(" %c", &odp);
    switch (odp)
    {
    case 't':
        printf("Podaj nick: ");
        scanf("%s", nickname);
        save_score(nickname, score(board, difficulty));
        printf("Dziękujemy za grę\n\n");
        break;
    case 'n':
        printf("Dziękujemy za grę\n\n");
        break;
    default:
        printf("Niepoprawna komenda\n");
        break;
    }

    display_scores();

    return;
}

void file_mode(char *filename)
{
    read_file(filename);
    return;
}

void print_usage(char *name)
{
    printf("Użycie: %s [-f filename]\n", name);
    printf("Uruchomienie gry w trybie interaktywnym: %s\n", name);
    printf("Uruchomienie gry z pliku: %s -f filename\n", name);
    return;
}
#include "game.h"

static char read_input(void)
{
    char  c;
    char  flush;

    if (read(0, &c, 1) <= 0)
        return ('\0');
    read(0, &flush, 1);     /* consume the newline that follows the letter */
    if (c >= 'a' && c <= 'z')
        c -= 32;            /* 'a'=97, 'A'=65: difference is 32 */
    return (c);
}

static void handle_lifeline(question_t *q, int *flags, int hidden[4],
                             char choice)
{
    int  i;
    int  removed;

    if (choice == '1' && *flags & 1) {         /* bit 0: 50:50 still available */
        removed = 0;
        for (i = 0; i < 4 && removed < 2; i++) {
            if (i != q->answer) {
                hidden[i] = 1;
                removed++;
            }
        }
        *flags &= ~1;                           /* clear bit 0: 50:50 spent */
        tci_printf("\n50:50 — two wrong answers removed.\n\n");
    } else if (choice == '2' && *flags & 2) {  /* bit 1: phone still available */
        if (q->hint)
            tci_printf("\nFriend says: %s\n\n", q->hint);
        else
            tci_printf("\nFriend says: The answer is %c.\n\n",
                       "ABCD"[q->answer]);
        *flags &= ~2;                           /* clear bit 1: phone spent */
    } else if (choice == '3' && *flags & 4) {  /* bit 2: audience still available */
        display_audience(q);
        *flags &= ~4;                           /* clear bit 2: audience spent */
    } else {
        tci_printf("Lifeline not available.\n");
    }
}

void    game_loop(question_t **questions, int count)
{
    int          level;
    int          safe_level;
    int          lifelines;
    int          hidden[4];
    int          i;
    char         c;
    question_t  *q;

    level      = 0;
    safe_level = -1;
    lifelines  = 7;     /* 0b111: bits 0=50:50, 1=phone, 2=audience — all available */
    while (level < LEVELS && level < count) {
        q = questions[level];
        for (i = 0; i < 4; i++)
            hidden[i] = 0;
        display_ladder(level, safe_level);
        if (SAFE[level])
            safe_level = level;     /* bank the floor prize for this level */
        tci_printf("Level %d — %s\n\n", level + 1, PRIZES[level]);
        display_question(q, hidden);
        tci_printf("[1] 50:50  [2] Phone  [3] Audience  "
                   "[W] Walk away\n");
        tci_printf("Your answer (A-D): ");
        while (1) {
            c = read_input();
            if (c == 'A' || c == 'B' || c == 'C' || c == 'D')
                break;
            if (c == '1' || c == '2' || c == '3' || c == 'W') {
                if (c == 'W') {
                    display_walkaway(level);
                    free_questions(questions, count);
                    return;
                }
                handle_lifeline(q, &lifelines, hidden, c);
                display_question(q, hidden);
                tci_printf("Your answer (A-D): ");
            }
        }
        if (c - 'A' == q->answer) {     /* 'A'-'A'=0, 'B'-'A'=1, 'C'-'A'=2, 'D'-'A'=3 */
            tci_printf("\nCorrect!\n");
            level++;
            if (level == LEVELS) {
                display_win();
                break;
            }
        } else {
            display_loss(safe_level);
            break;
        }
    }
    free_questions(questions, count);
}

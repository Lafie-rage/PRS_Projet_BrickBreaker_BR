/**
 * /file kaasbriik.c
 * /author Corentin DESTREZ & Valentin GUIBERTEAU
 * /date 12 Apr 2021
 * /brief Source code of the client's library
 *
 * This library containts the definition of some useful values, functions
 * and structures that are used by the server.
 *
 */

#include <string.h>

/*****************************************************************************/
/*                              CUSTOM LIBRARIES                             */
/*****************************************************************************/
#include "kassbriik.h"

void sortScoreList(t_scores_list *scores) {
  // Implementing a quick sort won't be really relevant because the list will be pretty short
  // and optimizing the algorithm is not the main point of the project.
  // That's why I choosed the bubble sort which is easier to implement.
  int i, j;
  t_score tmpScore; // Temporary var to swap scores
  for(i = 0; i < scores->size; i++) {
    for(j = 0; j < i; j++) {
      if(scores->scores[j+1].score < scores->scores[j].score) {
        tmpScore = scores->scores[j];
        scores->scores[j] = scores->scores[j+1];
        scores->scores[j+1] = tmpScore;
      }
    }
  }
}

void copyScoreList(t_scores_list *dest, t_scores_list src) {
  int i;
  dest->size = src.size;
  for(i = 0; i < dest->size; i++) {
    dest->scores[i].score = src.scores[i].score;
    strcpy(dest->scores[i].username, src.scores[i].username);
  }
}

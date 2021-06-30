#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "chat1002.h"

/* [TAKE NOTE THIS IS IMPORTANT!!!]
 * Get the response to a question.
 *
 * Input:
 *   intent   - the question word
 *   entity   - the entity
 *   response - a buffer to receive the response
 *   n        - the maximum number of characters to write to the response buffer
 *
 * Returns:
 *   KB_OK, if a response was found for the intent and entity (the response is copied to the response buffer)
 *   KB_NOTFOUND, if no response could be found
 *   KB_INVALID, if 'intent' is not a recognised question word
 */

/* This function retrieves a response to the question asked to BOOMERBot
*  It does so by utilising linked list, using the intent of "WHO", "WHAT", "WHERE" 
*  , entities and also response as its core variables. 
*
*   It checks on the existence on the intent and entity and get the response
*   If no response found in list, will go to knowledge_put(); instead to save and learn response
*/
int knowledge_get(const char *intent, const char *entity, char *response, int n) {

  /* intent is "WHO" */
  if (compare_token(intent, "who") == 0) {

    if (head_who != NULL) { // if linked list exists
        temp_who = head_who;
        while (temp_who) {
          if (compare_token(temp_who->entity, entity) == 0) {   // check from linked list for a match of entity
            strncpy(response, temp_who->response, n); // copy string from linked list to response and display
            return KB_OK;
          }
          temp_who = temp_who->next; // point to the next 'node' if no match
        }
        return KB_NOTFOUND; // all string checked for entities and no match found, bot will ask for appropriate response from user here
    
    } else {
        return KB_NOTFOUND; // this is different from above as this is to check the intent is present
    }
  
  /* Rinse and repeat for 'WHAT' */
  } else if (compare_token(intent, "what") == 0) {

    if (head_what != NULL) {
      temp_what = head_what;
      while (temp_what) {
        if (compare_token(temp_what->entity, entity) == 0) {                 
          strncpy(response, temp_what->response, n);
          return KB_OK;
        }
        temp_what = temp_what->next;
      }
      return KB_NOTFOUND;
    
    } else {
      return KB_NOTFOUND;
    }
  
  /* same thing for "WHERE" */
  } else if (compare_token(intent, "where") == 0) {

    if (head_where != NULL) {
      temp_where = head_where;
      while (temp_where) {
        if (compare_token(temp_where->entity, entity) == 0) {
          strncpy(response, temp_where->response, n);
          return KB_OK;
        }
        temp_where = temp_where->next;
      }
      return KB_NOTFOUND;

    } else {
      return KB_NOTFOUND;
    }
  
  } else {
    return KB_INVALID;
  }
}


/*
 * This function insert a new response to a question asked from the user by
 * matching the intent and entity of the question. Overwriting the response of the question
 * and also learning new response. 
 * 
 */
int knowledge_put(const char *intent, const char *entity, const char *response) {

  char buffer[MAX_RESPONSE];
  memset(buffer, '\0', MAX_RESPONSE); // preset buffer as '/0'

  if (abs(knowledge_get(intent, entity, buffer, MAX_RESPONSE)) == 2) {
    return KB_INVALID; //intent is invalid from knowledge get
  
  } else {   

    /* intent is "WHO" */
    if (compare_token(intent, "who") == 0) {

      if (abs(knowledge_get(intent, entity, buffer, MAX_RESPONSE)) == 0) {
        // the return value from knowledge_get();
        temp_who = head_who;
        while (temp_who) {
          if (compare_token(temp_who->entity, entity) == 0) {
            strncpy(temp_who->response, response, MAX_RESPONSE); // overwriting the existing response
            return KB_OK;
          }
          temp_who = temp_who->next;
        }

      } else if (abs(knowledge_get(intent, entity, buffer, MAX_RESPONSE)) == 1) { // from knowledge_get(); if no records for entity exist
        node_ptr temp = malloc(sizeof(node_properties));
      
        if (temp == NULL) {
          return KB_NOMEM; // in the event if runs out of memory
        }
        // setting and copying each properties to memory
        strcpy(temp->intent, intent);
        strcpy(temp->entity, entity);
        strcpy(temp->response, response);
        temp->next = head_who;
        head_who = temp;

        return KB_OK;
      }

    /* rinse and repeat for "WHAT" */
    } else if (compare_token(intent, "what") == 0) {

      if (abs(knowledge_get(intent, entity, buffer, MAX_RESPONSE)) == 0) {
        temp_what = head_what;
        while (temp_what) {
          if (compare_token(temp_what->entity, entity) == 0){
            strncpy(temp_what->response, response, MAX_RESPONSE);
            return KB_OK;
          }
          temp_what = temp_what->next;
        }
      
      } else if (abs(knowledge_get(intent, entity, buffer, MAX_RESPONSE)) == 1) {
        node_ptr temp = malloc(sizeof(node_properties));

        if (temp == NULL) {
          return KB_NOMEM;
        }

        strcpy(temp->intent, intent);
        strcpy(temp->entity, entity);
        strcpy(temp->response, response);
        temp->next = head_what;
        head_what = temp;
        return KB_OK;
      }
    
    /* rinse and repeat for "WHERE" */
    } else if (compare_token(intent, "where") == 0) {

      if (abs(knowledge_get(intent, entity, buffer, MAX_RESPONSE)) == 0) {
        temp_where = head_where;
        while (temp_where) {
          if (compare_token(temp_where->entity, entity) == 0) {
            strncpy(temp_where->response, response, MAX_RESPONSE);
            return KB_OK;
          }
          temp_where = temp_where->next;
        } 
      
      } else if (abs(knowledge_get(intent, entity, buffer, MAX_RESPONSE)) == 1) {
        node_ptr temp = malloc(sizeof(node_properties));

        if (temp == NULL) {
            return KB_NOMEM;
        }

        strcpy(temp->intent, intent);
        strcpy(temp->entity, entity);
        strcpy(temp->response, response);
        temp->next = head_where;
        head_where = temp;
        return KB_OK;
      }
    }
  }
}


/*
 * Read a knowledge base from a file.
 *
 * Input:
 *   f - the file
 *
 * Returns: the number of entity/response pairs successful read from the file
 */
int knowledge_read(FILE *f) {

  /*
	 * ctr:	The number of successful results retrieved from the file.
	 * result:	The result of inserting the entity-response into the knowledge base.
	 * line:	The buffer to store content of the file.
	 */

  int ctr = 0, result;
  char line[MAX_INPUT];
  char *f_intent;

  const size_t MAX_READLEN = (size_t)(MAX_ENTITY + MAX_RESPONSE);

  while (fgets(line, MAX_READLEN, (FILE *)f)) {
    char *f_entity, *f_response;
    char *cr, *nl;

    // Read line by line.
    if (strcmp(line, "\n") == 0 || strcmp(line, "\r\n") == 0) {
      // Empty line.
      continue;

    } else {
      /*search for first occurence of carriage return*/
      cr = strchr(line, '\r');
      /*search for first occurence of line feed */
      nl = strchr(line, '\n');

      if (cr != NULL) {
        // Carriage return found, replace it with null terminator.
        *cr = '\0';
      } else if (nl != NULL) {
        // newline found, replace it with null terminator.
        *nl = '\0';
      } else {
        // Clear any remaining input to prevent overflow.
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {
          continue;
        }
      }

      if (strchr(line, '[') != NULL && strchr(line, ']') != NULL) {
        // Square brackets found. Check intent.
        if (compare_token(line, "[what]") == 0) {
          // Intent: what.
          f_intent = "what";
        } else if (compare_token(line, "[where]") == 0) {
          // Intent: where.
          f_intent = "where";
        } else if (compare_token(line, "[who]") == 0) {
          // Intent: who.
          f_intent = "who";
        } else {
          // Invalid intent.
          f_intent = NULL;
        }

      } else if (f_intent != NULL && strchr(line, '=') != NULL) {
        // Entity-Response pair line.
        f_entity = strtok(line, "=");
        f_response = strtok(NULL, "=");
        result = knowledge_put(f_intent, f_entity, f_response);

        if (result == KB_OK) {
          // Increment the successful counter.
          ctr++;
        }
      }
    }
  }
  return ctr;
}


/*
 * Reset the knowledge base, removing all know entitities from all intents.
 */
void knowledge_reset() {
  /*
  * Check the head_what, head_who and head_where is it null.
  * If null, straight return back to chatbot_do_reset.
  * Initialize the temp with head
  * Make the head point to the next node
  * Free the temp
  * Go back to step 1 (check the head whether is it null)
  */

  /* Empty linked list for what */

  // While head_what is not NULL
  while (head_what != NULL) {
    // Initialize temp_what with head_what
    temp_what = head_what;
    // Point head_what to the next node
    head_what = head_what->next;
    // Free the node of temp_what
    free(temp_what);
    // Set temp_what to NULL
    temp_what = NULL;
  }

  /* Empty linked list for where */

  // While head_what is not NULL
  while (head_where != NULL) {
    // Initialize temp_where with head_where
    temp_where = head_where;
    // Point head_where to the next node
    head_where = head_where->next;
    // Free the node of temp_where
    free(temp_where);
    // Set temp_where to NULL
    temp_where = NULL;
  }

  /*Empty linked list for who */

  // While head_what is not NULL
  while (head_who != NULL) {
    // Initialize temp_who with head_who
    temp_who = head_who;
    // Point head_who to the next node
    head_who = head_who->next;
    // Free the node of temp_who
    free(temp_who);
    // Set temp_who to NULL
    temp_who = NULL;
  }
}


/*
 * Write the knowledge base to a file.
 *
 * Input:
 *   f - the file
 */
void knowledge_write(FILE *f) {

  // if file pointer points to null, print error message
  if (f == NULL) {
    printf("Sorry, unable to create file.\n");

  /* else save the data into file in sequences (what -> where -> who)
   * for each intent, get the corresponding linked list and 
   * save the entity and response in correct format (entity=reponse) into the file
   */
  } else {

    /* rinse and repeat for "WHAT" */
    fprintf(f, "[what]\n");

    // if the linked list exists, use a temp pointer to point to the head 
    // else it will jump to the next intent (e.g. "WHERE")
    if (head_what != NULL) {
      temp_what = head_what;
      while (temp_what != NULL) { // loop until the temp pointer == NULL
        fprintf(f, "%s=%s\n", temp_what->entity, temp_what->response); // save the entity and response for "WHAT" intent into the file
        temp_what = temp_what->next;  // point to the next node
      }
    }

    /* rinse and repeat for "WHERE" */
    fprintf(f, "\n[where]\n");
    if (head_where != NULL) {
      temp_where = head_where;
      while (temp_where != NULL) {
        fprintf(f, "%s=%s\n", temp_where->entity, temp_where->response);
        temp_where = temp_where->next; 
      }
    }

    /* rinse and repeat for "WHO" */
    fprintf(f, "\n[who]\n");
    if (head_who != NULL) {
      temp_who = head_who;
      while (temp_who != NULL) {
        fprintf(f, "%s=%s\n", temp_who->entity, temp_who->response);
        temp_who = temp_who->next; 
      }
    }

    // close the file pointer after putting all the contents into the file
    fclose(f);
  }
}
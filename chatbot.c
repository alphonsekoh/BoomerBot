/*
 * [Things to take note]
 * Input parameters:
 *   inc      - the number of words in the question
 *   inv      - an array of pointers to each word in the question
 *   response - a buffer to receive the response
 *   n        - the size of the response buffer
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "chat1002.h"

/*
 * Get the name of the chatbot.
 *
 * Returns: the name of the chatbot as a null-terminated string
 */
const char *chatbot_botname() {

	return "BoomerBOT";
}

/*
 * Get the name of the user.
 *
 * Returns: the name of the user as a null-terminated string
 */
const char *chatbot_username() {

	return getenv("USERNAME"); //gets the PC username
}

/*
 * Get a response to user input.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0, if the chatbot should continue chatting
 *   1, if the chatbot should stop (i.e. it detected the EXIT intent)
 */
int chatbot_main(int inc, char *inv[], char *response, int n) {

	/* check for empty input */
	if (inc < 1) {
		snprintf(response, n, "");
		return 0;
	}

	/* look for an intent and invoke the corresponding do_* function */
	if (chatbot_is_exit(inv[0]))
		return chatbot_do_exit(inc, inv, response, n);
	else if (chatbot_is_smalltalk(inv[0]))
		return chatbot_do_smalltalk(inc, inv, response, n);
	else if (chatbot_is_load(inv[0]))
		return chatbot_do_load(inc, inv, response, n);
	else if (chatbot_is_question(inv[0]))
		return chatbot_do_question(inc, inv, response, n);
	else if (chatbot_is_reset(inv[0]))
		return chatbot_do_reset(inc, inv, response, n);
	else if (chatbot_is_save(inv[0]))
		return chatbot_do_save(inc, inv, response, n);
	else {
		snprintf(response, n, "I don't understand \"%s\".", inv[0]);
		return 0;
	}
}

/*
 * Determine whether an intent is EXIT.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "exit" or "quit"
 *  0, otherwise
 */
int chatbot_is_exit(const char *intent) {

	return compare_token(intent, "exit") == 0 || compare_token(intent, "quit") == 0 || compare_token(intent, "bye") == 0;
}

/*
 * Perform the EXIT intent.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after a question)
 */
int chatbot_do_exit(int inc, char *inv[], char *response, int n) {

	snprintf(response, n, "Goodbye!");
	return 1;
}

/*
 * Determine whether an intent is LOAD.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "load"
 *  0, otherwise
 */
int chatbot_is_load(const char *intent) {

	/*Compare if the intent of the user is load*/
	return compare_token(intent, "load") == 0;
}

/*
 * Load a chatbot's knowledge base from a file.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after loading knowledge)
 */
int chatbot_do_load(int inc, char *inv[], char *response, int n) {

	/*
		fp:		The file pointer.
		ctr:	The number of successful results retrieved from the file.
	*/
	FILE *fp;
	int ctr = 0;
	char file_path[MAX_INPUT];

	// Get the file path from the user input.
	if (compare_token(inv[1], "from") == 0) {
		/*If the second word the user specify "from", 
		we copy the sentence after the "from" as the file path.*/
		strcpy(file_path, inv[2]);
	} else {
		/*If the user only specify "Load", we copy the sentence after the "load" as the file path.*/
		strcpy(file_path, inv[1]);
	}

	// Open the file in read mode.
	fp = fopen(file_path, "r");

	if (fp != NULL) {
		// File exists.
		ctr = knowledge_read(fp);
    // Close the file.
		fclose(fp);
		snprintf(response, n, "I have loaded %d results from the knowledge base [%s].", ctr, file_path);
	} else {
		// File does not exist.
		snprintf(response, n, "Sorry, I can't load the knowledge base [%s].", file_path);
	}

	return 0;
}

/*
 * Determine whether an intent is a question.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "what", "where", or "who"
 *  0, otherwise
 */
int chatbot_is_question(const char *intent) {
	
	if (compare_token(intent, "what") == 0 || compare_token(intent, "where") == 0 || compare_token(intent, "who") == 0) {
			return 1;
	}
	return 0;
}

/*
 * Answer a question.
 *
 * inv[0] contains the the question word.
 * inv[1] may contain "is" or "are"; if so, it is skipped.
 * The remainder of the words form the entity.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after a question)
 */
int chatbot_do_question(int inc, char *inv[], char *response, int n) {

	char input[MAX_INPUT];  /* Buffer for holding the user input*/
	char intent[MAX_INTENT]; /* Buffer for holding the intent*/
	char entity[MAX_ENTITY];	/* Buffer for holding entity*/
	char chat[MAX_RESPONSE]; /* the chatbot's response*/
	char bad_chat[MAX_RESPONSE]; /* the response from the user to store in knowledge base*/
	int know_get, know_put; /* Return value from the knowledge_get and knowledge_put*/
	
	strcpy(input, "\0");
  
  // Get the intent from the question
	strncpy(intent, inv[0], sizeof(intent) / sizeof(intent[0]));
  
	snprintf(entity, n, "");
	
  /* If only 1 inc, means there is only a word in the question
   * and print the error message according to the intent
   */
	if (inc == 1) {
    
		if (compare_token(intent, "what") == 0) {
			snprintf(response, n, "Sorry, I did not understand the phrase. Did you mean something like, \"What is ICT?\"");
      
		} else if (compare_token(intent, "who") == 0) {
		  snprintf(response, n, "Sorry, I did not understand the phrase. Did you mean something like, \"Who is the cluster director of ICT?\"");
      
		} else if (compare_token(intent, "where") == 0) {
      snprintf(response, n, "Sorry, I did not understand the phrase. Did you mean something like, \"Where is SIT?\"");
      
		}
		return 0;
    
  /* If only 2 inc, check if the second word whether is a "is" or "are"
   * and print the error message if the condition is true or 1.
   */
	} else if ((inc == 2 && compare_token(inv[1], "is") == 0) || (inc == 2 && compare_token(inv[1], "are") == 0)) {
    snprintf(response, n, "Sorry, I did not understand the phrase. Please describe your noun.");
    return 0;
	}
	
  // Checks for adjectives between intent and entity "is" and "are"
	for (int i = 1; i < inc; i++) {
    if ((i == 1 && compare_token(inv[i], "is") == 0) || (i == 1 && compare_token(inv[i], "are") == 0)) {
      strncpy(input, inv[i], sizeof(inv[i]) / sizeof(inv[i][0]));
      continue;
    }
    // Store into entity
    strcat(strcat(entity, " "), inv[i]); 
	}
  
  // Memmove over memcpy cause can overlap
	memmove(entity, entity + 1, strlen(entity)); 
	
  // Inovke the knowledge_get() function
	know_get = knowledge_get(intent, entity, chat, n);
  
  /* Success in knowledge_get() 
   * and print the response get from the knowledge base to user
   */ 
	if (know_get == KB_OK) {
			snprintf(response, n, "%s", chat);
    
  /* There is no response found in knowledge base */
	} else if (know_get == KB_NOTFOUND) {
		
    // Prompt the user for the response to store in knowledge base 
    if (compare_token(input, "\0") == 0) {
				prompt_user(bad_chat, MAX_INPUT, "I don't know. %s %s?", intent, entity);
		} else {
				prompt_user(bad_chat, MAX_INPUT, "I don't know. %s %s %s?", intent, input, entity); 
      
		}
    
    // If the response is empty, set the response to "-(" and print this response to use
		if (strcmp(bad_chat, "") == 0) {
				strcpy(response, "-(");
      
		} else {
      // Invoke the knowledge_put function
      know_put = knowledge_put(intent, entity, bad_chat);
      
      // Success in knowledge_put() and print the success message
      if (know_put == KB_OK) {
        snprintf(response, n, "Thank you.");
        
      // Insufficient data in heap. Print the error message   
      } else if (know_put == KB_NOMEM) {
        snprintf(response, n, "Could not allocate memory! Data creation failed:\nIntent '%s'\nEntity '%s'\nResponse '%s'\n", intent, entity, bad_chat);
        exit(1);
        
      // The intent is invalid. Print the error message
      } else if (know_put == KB_INVALID) {
        snprintf(response, n, "Sorry, I didn't get '%s'.", intent);
        
      }
		}
  
  // The intent is not a valid question word. Print the error message
	} else if (know_get == KB_INVALID) {
			snprintf(response, n, "Sorry, I didn't get '%s'.", intent);
	}

	return 0;
}

/*
 * Determine whether an intent is RESET.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "reset"
 *  0, otherwise
 */
int chatbot_is_reset(const char *intent) {

	return compare_token(intent, "reset") == 0;
}

/*
 * Reset the chatbot.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after beign reset)
 */
int chatbot_do_reset(int inc, char *inv[], char *response, int n) {

	// Call the knowledge_reset function to reset the knowledge base.
	knowledge_reset();
	snprintf(response, n, "Chatbot reset.");
	return 0;
}

/*
 * Determine whether an intent is SAVE.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "save"
 *  0, otherwise
 */
int chatbot_is_save(const char *intent) {

  if (compare_token(intent, "save") == 0) {
    return 1;
	}
	return 0;
}

/*
 * Save the chatbot's knowledge to a file.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after saving knowledge)
 */
int chatbot_do_save(int inc, char *inv[], char *response, int n) {

  // if only 2 inc, means correct format = save xxx.ini
  if(inc == 2){

    // check whether the filename contain a "."
    char *fileformat = strrchr(inv[1], '.');

    // if file format is correct (.ini file), save knowledge into file and print success message
    if (fileformat && compare_token(fileformat, ".ini")==0) {
      FILE *f = fopen(inv[1], "w"); 	// create file pointer
      knowledge_write(f);
      snprintf(response, n, "My knowledge has been saved to %s",inv[1]);
      return 0;

    // else, print error message to indicate invalid file format
    } else {
      snprintf(response, n, "Sorry, the file format is invalid. Please save the file as an \".ini\" file.");
      return 0;
    }
  }

  // if 3 inc, means correct format = save as xxx.ini OR save to xxx.ini
  if((inc == 3) && ((compare_token(inv[1], "as") == 0) || (compare_token(inv[1], "to") == 0))) {

    // check whether the filename contain a "."
    char *fileformat = strrchr(inv[2], '.');

    // if file format is correct (.ini file), save knowledge into file and print success message
    if (fileformat && compare_token(fileformat, ".ini")==0) {
      FILE *f = fopen(inv[2], "w"); 	// create file pointer
      knowledge_write(f);
      snprintf(response, n, "My knowledge has been saved to %s",inv[2]);
      return 0;

    // else, print error message to indicate invalid file format
    } else {
        snprintf(response, n, "Sorry, the file format is invalid. Please save the file as an \".ini\" file.");		
        return 0;
    }
  }

  // if it does not match any criteria, print error message to suggest the proper command for "save" intent
  snprintf(response, n, "Sorry, I did not understand the phrase. Did you mean something like, \"save as sample.ini\"");
  return 0;
}

/*
 * Determine which an intent is smalltalk.
 *
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is the first word of one of the smalltalk phrases
 *  0, otherwise
 */
int chatbot_is_smalltalk(const char *intent){

  // implementation of smalltalk with Profanity Warning
  /* Store the intent from user into an array of pointers*/
	const char *smalltalk[] = {
	            "good", "hello", "hey", "hi", "it", "its", "it's","wassup","sup","yo","fuck","fucking","fucked", "shit", "shitting", "crap", "kanina"
  };
  
	/*Declare length of array*/
  size_t length = sizeof(smalltalk)/sizeof(smalltalk[0]);
	
  /*Create a loop to loop through the array to compare each intent whether 
	is it in smalltalk[]. If yes, return 1*/
	for (int i = 0; i < length; i++) {
		if (compare_token(intent, smalltalk[i]) == 0) {
			return 1;
		}
	}
  
	return 0;
}

/*
 * Respond to smalltalk.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0, if the chatbot should continue chatting
 *   1, if the chatbot should stop chatting (e.g. the smalltalk was "goodbye" etc.)
 */
int chatbot_do_smalltalk(int inc, char *inv[], char *response, int n) {
  
  /* Create an array of pointers to store all responses to smalltalk */
	const char *random_hi[] = {"Hi!", "Hello!", "Hello there!", "Hey hey~", "What's Up!!"};
	
  /* random number generator */
	int rand_int = (int)(rand() % 5);

	if (compare_token("good", inv[0]) == 0) {
		/* Check if the user specify "good" as the first word */
		if (inc > 1) {
      /* If the user specify something else after "good", the chatbot will copy the word
			after "good" and respond with it */
			snprintf(response, n, "Good %s to you too, %s!", inv[1], getenv("USERNAME"));
		} else {
      /* If the user only specify "good", the chatbot will respond with "Good day!" */
			snprintf(response, n, "Good day!");
		}
    
	} else if (compare_token("hello", inv[0]) == 0 || compare_token("hey", inv[0]) == 0 || compare_token("hi", inv[0]) == 0) {
		/* If the user first word is any of the above, the bot will randomly generate a number
		and retrieve the respond from random_hi array */
		snprintf(response, n, "%s %s", random_hi[rand_int], getenv("USERNAME"));
    
	} else if (compare_token("it", inv[0]) == 0 || compare_token("its", inv[0]) == 0 || compare_token("it's", inv[0]) == 0) {
		/* If the user first word is any of the above, the bot will respond with "Indeed it is." */
		snprintf(response, n, "Indeed it is.");
    
	} else if (compare_token("fuck", inv[0]) == 0 || compare_token("fucking", inv[0]) == 0 || compare_token("fucked", inv[0]) == 0 || compare_token("shit", inv[0]) == 0 || compare_token("shitting", inv[0]) == 0 || compare_token("crap", inv[0]) == 0 || compare_token("kanina", inv[0]) == 0 ) {
    // Profanity warning from the bot
    snprintf(response, n, "That's not a very nice thing to say!");
	}
  
	return 0;
}
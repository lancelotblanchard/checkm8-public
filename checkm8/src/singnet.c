#include <stdlib.h>
#include <stdio.h>
#include "singnet.h"
#include <string.h>

#define max_cmd_length 200
#define max_response_length 300
#define empty_string "\0"
#define game_id "checkm8"

void alpha_make_move(char *move)
{
  Alpha_Request *request = malloc(sizeof(Alpha_Request));
  init_alpha_request(request);

  request->move = move;

  FILE *res_file = snet_alpha_request(request); 

  char *res_buffer = calloc(sizeof(char), max_response_length);
  if(res_file == NULL)
  {
    perror("Error: response is NULL");
  }

  char res_move[4];
  if(fscanf(res_file, "status: \"move_error\""))
  {
    perror("Invalid move, please enter again: ");
  }
  else if(fscanf(res_file, "status: \"game_running: %s\"", res_move))
  {
    printf("\n\nyou've made this move: %s\n\n", res_move);
  }
  else{
    printf("pattern match failed\n");
  }
  
  fclose(res_file);
  free(res_buffer);
  free(request);
}

void alpha_reset(void)
{
  printf("Resetting board...\n");

  Alpha_Request *request = malloc(sizeof(Alpha_Request));
  init_alpha_request(request);
  
  request->cmd = "reset";
  FILE *res_file = snet_alpha_request(request);
  

  if(res_file == NULL)
  {
    perror("Error: response is NULL");
  }

  free(request);
}

FILE *snet_alpha_request(Alpha_Request *request)
{
  char *alpha_out = "../resources/alpha_out.txt";

  char *call = "snet client call -y snet zeta36-chess-alpha-zero play";
  char *json = calloc(sizeof(char), max_cmd_length);

  snprintf(json, max_cmd_length,
   "{\"uid\": \"%s\", \"move\": \"%s\", \"cmd\": \"%s\"}",
   request->uid, request->move, request->cmd);
  printf("\n\n%s\n\n", json);
  char *cmd = calloc(sizeof(char), max_cmd_length);

  snprintf(cmd, max_cmd_length,
   "{ printf \"\n$(%s '%s')\n\"; } > %s",
   call, json, alpha_out);

  if(system(cmd) == -1)
  {
    perror("Error: failed to send request");
  }
  
  FILE *res_file = fopen(alpha_out, "r");
  
  free(cmd); 
  free(json);
  return res_file;
}

void init_alpha_request(Alpha_Request *request)
{
  request->uid = game_id;
  request->move = "\0";
  request->cmd = "\0";
}
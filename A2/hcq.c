#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "hcq.h"
#define INPUT_BUFFER_SIZE 256

/*
 * Return a pointer to the struct student with name stu_name
 * or NULL if no student with this name exists in the stu_list
 */
Student *find_student(Student *stu_list, char *student_name) {
    struct student *current = stu_list;
    if (current == NULL) {
      return NULL;
    }
    while (current != NULL) {
      if(strcmp(current->name, student_name) == 0) {
        return current;
      }
      current = current->next_overall;
    }
    return NULL;
}



/*   Return a pointer to the ta with name ta_name or NULL
 *   if no such TA exists in ta_list.
 */
Ta *find_ta(Ta *ta_list, char *ta_name) {
    struct ta *current = ta_list;
    if (current == NULL) {
      return NULL;
    }
    while (current != NULL) {
      if(strcmp(current->name, ta_name) == 0) {
        return current;
      }
      current = current->next;
    }
    return NULL;
}


/*  Return a pointer to the course with this code in the course list
 *  or NULL if there is no course in the list with this code.
 */
Course *find_course(Course *courses, int num_courses, char *course_code) {
  for (int i = 0; i < num_courses; i++) {
    if (strncmp(course_code, courses[i].code, 7) == 0) {
      return &courses[i];
    }
  }
    return NULL;
}


/* Add a student to the queue with student_name and a question about course_code.
 * if a student with this name already has a question in the queue (for any
   course), return 1 and do not create the student.
 * If course_code does not exist in the list, return 2 and do not create
 * the student struct.
 * For the purposes of this assignment, don't check anything about the
 * uniqueness of the name.
 */
int add_student(Student **stu_list_ptr, char *student_name, char *course_code,
    Course *course_array, int num_courses) {
    struct course *c_ptr;


    if(find_student(*stu_list_ptr, student_name) != NULL) {
      return 1;
    }

    c_ptr = find_course(course_array, num_courses, course_code);
    if (c_ptr == NULL) {
      return 2;
    }

    //Create a new student.
    struct student *s = malloc(sizeof(struct student));
    s->name = malloc(strlen(student_name) + 1);
    strcpy(s->name, student_name);
    s->arrival_time = malloc(sizeof(time_t));
    time(s->arrival_time);
    s->course = c_ptr;
    s->next_overall = NULL;
    s->next_course = NULL;

    if(*stu_list_ptr == NULL) {
      *stu_list_ptr = s;
    } else {
      struct student *last = *stu_list_ptr;
      while (last->next_overall != NULL) {
        last = last->next_overall;
      }
      last->next_overall = s;
    }

    if(c_ptr->tail != NULL) {
      (c_ptr->tail)->next_course = s;
      c_ptr->tail= s;
    } else {
      c_ptr->tail = s;
      c_ptr->head = s;
    }

    return 0;
}


/* Student student_name has given up waiting and left the help centre
 * before being called by a Ta. Record the appropriate statistics, remove
 * the student from the queues and clean up any no-longer-needed memory.
 *
 * If there is no student by this name in the stu_list, return 1.
 */
int give_up_waiting(Student **stu_list_ptr, char *student_name) {
    struct student *target = *stu_list_ptr;
    struct student *previous = *stu_list_ptr;
    struct student *previous_course = *stu_list_ptr;

    target = find_student(*stu_list_ptr, student_name);
    if (target == NULL){
      return 1;
    }

    if(target == *stu_list_ptr) {
      (target->course)->head = target->next_course;
      *stu_list_ptr = target->next_overall;
      ((target->course)->bailed)++;
      free(target->name);
      free(target->arrival_time);
      free(target);
      return 0;
    }

    if (target == (target->course)->head) {
      (target->course)->head = target->next_course;
      previous->next_overall = target->next_overall;
      ((target->course)->bailed)++;
      free(target->name);
      free(target->arrival_time);
      free(target);
      return 0;
    }

    while (previous->next_overall != NULL) {
      if (strcmp((previous->next_overall)->name, student_name) == 0) break;
      previous = previous->next_overall;
    }
    while (previous_course->next_overall != NULL) {
      if (strcmp((previous_course->next_course)->name, student_name) != 0) break;
      previous_course = previous_course->next_overall;
    }

    previous->next_overall = target->next_overall;
    if (strcmp(((target->course)->tail)->name, student_name) == 0){
      (target->course)->tail = previous_course;
      previous_course->next_course = NULL;
    } else {
      previous_course->next_course = target->next_course;
    }
    ((target->course)->bailed)++;
    free(target->name);
    free(target->arrival_time);
    free(target);
    return 0;
}

/* Create and prepend Ta with ta_name to the head of ta_list.
 * For the purposes of this assignment, assume that ta_name is unique
 * to the help centre and don't check it.
 */
void add_ta(Ta **ta_list_ptr, char *ta_name) {
    // first create the new Ta struct and populate
    Ta *new_ta = malloc(sizeof(Ta));
    if (new_ta == NULL) {
       perror("malloc for TA");
       exit(1);
    }
    new_ta->name = malloc(strlen(ta_name)+1);
    if (new_ta->name  == NULL) {
       perror("malloc for TA name");
       exit(1);
    }
    strcpy(new_ta->name, ta_name);
    new_ta->current_student = NULL;

    // insert into front of list
    new_ta->next = *ta_list_ptr;
    *ta_list_ptr = new_ta;
}

/* The TA ta is done with their current student.
 * Calculate the stats (the times etc.) and then
 * free the memory for the student.
 * If the TA has no current student, do nothing.
 */
void release_current_student(Ta *ta) {
  struct student *s = ta->current_student;
  time_t now;
  time(&now);
  if (s != NULL){
    (s->course)->helped++;
    (s->course)->help_time += (now - *(s->arrival_time));
    free(s->name);
    free(s->arrival_time);
    free(s);
    ta->current_student = NULL;
  }
}

/* Remove this Ta from the ta_list and free the associated memory with
 * both the Ta we are removing and the current student (if any).
 * Return 0 on success or 1 if this ta_name is not found in the list
 */
int remove_ta(Ta **ta_list_ptr, char *ta_name) {
    Ta *head = *ta_list_ptr;
    if (head == NULL) {
        return 1;
    } else if (strcmp(head->name, ta_name) == 0) {
        // TA is at the head so special case
        *ta_list_ptr = head->next;
        release_current_student(head);
        // memory for the student has been freed. Now free memory for the TA.
        free(head->name);
        free(head);
        return 0;
    }
    while (head->next != NULL) {
        if (strcmp(head->next->name, ta_name) == 0) {
            Ta *ta_tofree = head->next;
            //  We have found the ta to remove, but before we do that
            //  we need to finish with the student and free the student.
            //  You need to complete this helper function
            release_current_student(ta_tofree);

            head->next = head->next->next;
            // memory for the student has been freed. Now free memory for the TA.
            free(ta_tofree->name);
            free(ta_tofree);
            return 0;
        }
        head = head->next;
    }
    // if we reach here, the ta_name was not in the list
    return 1;
}






/* TA ta_name is finished with the student they are currently helping (if any)
 * and are assigned to the next student in the full queue.
 * If the queue is empty, then TA ta_name simply finishes with the student
 * they are currently helping, records appropriate statistics,
 * and sets current_student for this TA to NULL.
 * If ta_name is not in ta_list, return 1 and do nothing.
 */
int take_next_overall(char *ta_name, Ta *ta_list, Student **stu_list_ptr) {
    struct ta *target = find_ta(ta_list, ta_name);
    if (target == NULL) return 1;
    struct student *s = *stu_list_ptr;
    time_t now;
    time(&now);

    release_current_student(target);
    if (*stu_list_ptr != NULL) {
      target->current_student = s;
      (s->course)->head = s->next_course;
      (s->course)->wait_time += (now - *(s->arrival_time));
      *stu_list_ptr = s->next_overall;
      s->next_overall = NULL;
      s->next_course = NULL;
      time(s->arrival_time);
    }
    return 0;
}



/* TA ta_name is finished with the student they are currently helping (if any)
 * and are assigned to the next student in the course with this course_code.
 * If no student is waiting for this course, then TA ta_name simply finishes
 * with the student they are currently helping, records appropriate statistics,
 * and sets current_student for this TA to NULL.
 * If ta_name is not in ta_list, return 1 and do nothing.
 * If course is invalid return 2, but finish with any current student.
 */
int take_next_course(char *ta_name, Ta *ta_list, Student **stu_list_ptr, char *course_code, Course *courses, int num_courses) {
  struct ta *target = find_ta(ta_list, ta_name);
  struct course *c = find_course(courses, num_courses, course_code);
  if (target == NULL) return 1;
  if (c == NULL) return 2;
  struct student *s = c->head;
  time_t now;
  time(&now);

  release_current_student(target);

  if (*stu_list_ptr != NULL) {
    target->current_student = s;
    (s->course)->head = s->next_course;
    (s->course)->wait_time = (s->course)->wait_time + (now - *(s->arrival_time));
    if (s == *stu_list_ptr) {
      *stu_list_ptr = s->next_overall;
    }
    s->next_overall = NULL;
    s->next_course = NULL;
    time(s->arrival_time);
  }
  return 0;
}


/* For each course (in the same order as in the config file), print
 * the <course code>: <number of students waiting> "in queue\n" followed by
 * one line per student waiting with the format "\t%s\n" (tab name newline)
 * Uncomment and use the printf statements below. Only change the variable
 * names.
 */
void print_all_queues(Student *stu_list, Course *courses, int num_courses) {
  for (int i = 0; i < num_courses; i++) {
    struct student *current = courses[i].head;
    int num = 0;
    while (current != NULL) {
      num++;
      current = current->next_course;
    }
    printf("%s: %d in queue\n", courses[i].code, num);
    current = courses[i].head;
    while (current != NULL) {
      printf("\t%s\n", current->name);
      current = current->next_course;
    }
  }

}


/*
 * Print to stdout, a list of each TA, who they are serving at from what course
 * Uncomment and use the printf statements
 */
void print_currently_serving(Ta *ta_list) {
    struct ta *current = ta_list;
    if (current == NULL) {
      printf("No TAs are in the help centre.\n");
    }

    while (current != NULL) {
      if (current->current_student == NULL) {
        printf("TA: %s has no student\n", current->name);
      }
      printf("TA: %s is serving %s from %s\n",current->name,
      (current->current_student)->name, ((current->current_student)->course)->code);
      current = current->next;
    }
}


/*  list all students in queue (for testing and debugging)
 *   maybe suggest it is useful for debugging but not included in marking?
 */
void print_full_queue(Student *stu_list) {

}

/* Prints statistics to stdout for course with this course_code
 * See example output from assignment handout for formatting.
 *
 */
int stats_by_course(Student *stu_list, char *course_code, Course *courses, int num_courses, Ta *ta_list) {

    // TODO: students will complete these next pieces but not all of this
    //       function since we want to provide the formatting

    struct course *found = find_course(courses, num_courses, course_code);
    struct ta *current_ta = ta_list;
    int students_being_helped = 0;

    if(found == NULL) {
      perror("Course cannot be found");
      exit(1);
    }

    struct student *current = found->head;
    int students_waiting = 0;

    while (current != NULL) {
      students_waiting++;
      current = current->next_course;
    }

    while (current_ta != NULL) {
      if (current_ta->current_student != NULL) {
        if (strcmp(((current_ta->current_student)->course)->code, course_code) == 0) {
          students_being_helped++;
        }
      }
      current_ta = current_ta->next;
    }



    // You MUST not change the following statements or your code
    //  will fail the testing.
    printf("%s:%s \n", found->code, found->description);
    printf("\t%d: waiting\n", students_waiting);
    printf("\t%d: being helped currently\n", students_being_helped);
    printf("\t%d: already helped\n", found->helped);
    printf("\t%d: gave_up\n", found->bailed);
    printf("\t%f: total time waiting\n", found->wait_time);
    printf("\t%f: total time helping\n", found->help_time);
    return 0;
}


/* Dynamically allocate space for the array course list and populate it
 * according to information in the configuration file config_filename
 * Return the number of courses in the array.
 * If the configuration file can not be opened, call perror() and exit.
 */
int config_course_list(Course **courselist_ptr, char *config_filename) {
  FILE *input_stream;
  char code[7];
  char description[INPUT_BUFFER_SIZE + 1];
  int course_num;
  int i;

  input_stream = fopen(config_filename, "r");
  if (input_stream == NULL) {
    perror("Error opening file");
    exit(1);
  }

  fscanf(input_stream, "%d", &course_num);
  *courselist_ptr = malloc(sizeof(struct course) * course_num);

  for (i = 0; i < course_num; i++) {
    fscanf(input_stream, "%6s %[^\n]s", code, description);
    strcpy((*courselist_ptr)[i].code, code);
    (*courselist_ptr)[i].description = malloc(sizeof(char) * (INPUT_BUFFER_SIZE + 1));
    strcpy((*courselist_ptr)[i].description, description);
    (*courselist_ptr)[i].head = NULL;
    (*courselist_ptr)[i].tail = NULL;
    (*courselist_ptr)[i].helped = 0;
    (*courselist_ptr)[i].bailed = 0;
    (*courselist_ptr)[i].wait_time = 0;
    (*courselist_ptr)[i].help_time = 0;
  }

  fclose(input_stream);
  return course_num;
}

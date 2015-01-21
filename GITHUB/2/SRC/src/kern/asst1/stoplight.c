/*stoplight.c
*
* * 31-1-2003 : GWA : Stub functions created for CS161 Asst1.
* *
* * NB: You can use any synchronization primitives available to solve
* * the stoplight problem in this file.
* */

/*
 * * 
 * * Includes
 * *
 * */
#include <types.h>
#include <lib.h>
#include <test.h>
#include <thread.h>

/*
 * *
 * * Constants
 * *
 * */

/*
 * * Number of cars created.
 * */

#define NCARS 20
struct lock *resources[4];//The four resources 0->SE,1->NE,2->NW,3->SW
int m=0;//The counter to terminate the parent thread of the cars
/*
 * *
 * * Function Definitions
 * *
 * */

/*
 * * gostraight()
 * *
 * * Arguments:
 * *      unsigned long cardirection: the direction from which the car
 * *              approaches the intersection.
 * *      unsigned long carnumber: the car id number for printing purposes.
 * *
 * * Returns:
 * *      nothing.
 * *
 * * Notes:
 * *      This function should implement passing straight through the
 * *      intersection from any direction.
 * *      Write and comment this function.
 * */

static
void
gostraight(unsigned long cardirection,
          unsigned long carnumber)
{
/*
 * * Avoid unused variable warnings.
 * */
       unsigned long dummy, number1 , number2;//variables used for swapping and sorting

       number1=cardirection;
       number2=((cardirection+1)%4);
             
 if(number1>number2)
       {
               dummy=number1;
               number1=number2;
               number2=dummy;
       }
       //Sequence is in ascending order to avoid deadlock
       display(cardirection,'S',carnumber,0); 
       lock_acquire(resources[number1]);
       lock_acquire(resources[number2]);
       display(cardirection,'S',carnumber,1);  
       lock_release(resources[number1]);
       lock_release(resources[number2]);
       display(cardirection,'S',carnumber,2);
       m++;
       (void) cardirection;
       (void) carnumber;
}




/*
 * * turnleft()
 * *
 * * Arguments:
 * *      unsigned long cardirection: the direction from which the car
 * *              approaches the intersection.
 * *      unsigned long carnumber: the car id number for printing purposes.
 * *
 * * Returns:
 * *      nothing.
 * *
 * * Notes:
 * *      This function should implement making a left turn through the 
 * *      intersection from any direction.
 * *      Write and comment this function.
 * */

static
void
turnleft(unsigned long cardirection,
        unsigned long carnumber)
{
/*
 * * Avoid unused variable warnings.
 * */
       unsigned long temp, number1,number2,number3;//variables used for swapping and sorting
       number1=cardirection;
       number2=(cardirection+1)%4;
       number3=(cardirection+2)%4;
       if (number1 > number2) {
               temp = number1;
                number1 = number2;
                number2 = temp;
       }

       if (number2 > number3) {
               temp = number2;
               number2 = number3;
               number3 = temp;
               }

       if (number1 > number2) {
               temp = number1;
               number1 = number2;
               number2 = temp;
       }
       //Sequence is in ascending order to avoid deadlock
       display(cardirection,'L',carnumber,0);
       lock_acquire(resources[number1]);
       lock_acquire(resources[number2]);
       lock_acquire(resources[number3]);
       display(cardirection,'L',carnumber,1);
       lock_release(resources[number1]);       
       lock_release(resources[number2]);
       lock_release(resources[number3]);
       display(cardirection,'L',carnumber,2);
       m++;
     (void) cardirection;
       (void) carnumber;
}

/*
 * * turnright()
 * *
 * * Arguments:
 * *      unsigned long cardirection: the direction from which the car
 * *              approaches the intersection.
 * *      unsigned long carnumber: the car id number for printing purposes.
 * *
 * * Returns:
 * *      nothing.
 * *
 * * Notes:
 * *      This function should implement making a right turn through the 
 * *      intersection from any direction.
 * *      Write and comment this function.
 * */

static
void
turnright(unsigned long cardirection,
         unsigned long carnumber)
{
/*
 * * Avoid unused variable warnings.
 * */

       (void) cardirection;
       (void) carnumber;
       display(cardirection,'R',carnumber,0);
       lock_acquire(resources[cardirection]);
       display(cardirection,'R',carnumber,1);
       lock_release(resources[cardirection]);
       display(cardirection,'R',carnumber,2);
       m++;
}

/*
 * * approachintersection()
 * *
 * * Arguments: 
 * *      void * unusedpointer: currently unused.
 * *      unsigned long carnumber: holds car id number.
 * *	
 * * Returns:
 * *      nothing.
 * *
 * * Notes:
 * *      Change this function as necessary to implement your solution. These
 * *      threads are created by createcars().  Each one must choose a direction
 * *      randomly, approach the intersection, choose a turn randomly, and then
 * *      complete that turn.  The code to choose a direction randomly is
 * *      provided, the rest is left to you to implement.  Making a turn
 * *      or going straight should be done by calling one of the functions
 * *      above.
 * */
static
void
approachintersection(void * unusedpointer,
                    unsigned long carnumber)
{
       int cardirection;
       int turnPossibility;//The three directions in which the car can possibly go

/*
 * * Avoid unused variable and function warnings.
 * */

       (void) unusedpointer;
       (void) carnumber;
       (void) gostraight;
       (void) turnleft;
       (void) turnright;

/*
 * * cardirection is set randomly.
 * * turnPossibility is set randomly.
 * */

       cardirection = random() % 4;
       turnPossibility = random() % 3;
       switch(turnPossibility){

       case 0: gostraight(cardirection,carnumber);
               break;
       case 1: turnright(cardirection,carnumber);
               break;
       case 2: turnleft(cardirection,carnumber);
               break;
       }
}

/*
 * * display()
 * *
 * * Arguments: 
 * *      unsigned long cardirection: the direction from which the car
 * *              approaches the intersection.
 * *      unsigned long carnumber: the car id number for printing purposes.
 * *      char turnPossibility:The three directions in which the car can possibly go
 * *      int n: The value of n represents: 0 -> Approaching Intersection 1-> In The Intersection 2->      
 * *	Leave the Intersection
 * *	
 * * Returns:
 * *      nothing.
 * *
 * * Notes:
 * *      The display function is used to display the Action, Car Source, Car Direction, 
 * *      Car number
 * *     
 * *     
 * */
void display(unsigned long cardirection, char turnPossibility,unsigned long carnumber, int n)
{
	char cardirec;
	switch(n)
	{
		case 0: // Approach
		{
		char cardirec;
		if(cardirection==0) 
		{
			cardirec='S';
			kprintf("Action: Approach     Car Number : %ld\t Source: %c\t destination: %c\n", carnumber,cardirec,turnPossibility);
		}
		if(cardirection==1) 
		{
			cardirec='E';
			kprintf("Action: Approach     Car Number : %ld\t Source: %c\t destination: %c\n", carnumber,cardirec,turnPossibility);
		}
		if(cardirection==2) 
		{
			cardirec='N';
			kprintf("Action: Approach     Car Number : %ld\t Source: %c\t destination: %c\n", carnumber,cardirec,turnPossibility);
		}
		if(cardirection==3) 
		{cardirec='W';
			kprintf("Action: Approach     Car Number : %ld\t Source: %c\t destination: %c\n", carnumber,cardirec,turnPossibility);
		}
		break;
		}
		case 1: //Intesection
		{
		if(cardirection==0)
		{
			cardirec='S';
			kprintf("Action: Intersection Car Number : %ld\t Source: %c\t destination: %c\n", carnumber,cardirec,turnPossibility);
		}
		if(cardirection==1)
		{	
			cardirec='E';
			kprintf("Action: Intersection Car Number : %ld\t Source: %c\t destination: %c\n", carnumber,cardirec,turnPossibility);
		}
		if(cardirection==2)
		{
			cardirec='N';
			kprintf("Action: Intersection Car Number : %ld\t Source: %c\t destination: %c\n", carnumber,cardirec,turnPossibility);
		}
		if(cardirection==3)
		{
			cardirec='W';
			kprintf("Action: Intersection Car Number : %ld\t Source: %c\t destination: %c\n", carnumber,cardirec,turnPossibility);
		}
		break;
		}
		case 2: // Leave
		{ 
		if(cardirection==0)
		{
			cardirec='S';
			kprintf("Action: Leave        Car Number : %ld\t Source: %c\t destination: %c\n", carnumber,cardirec,turnPossibility);
		}
		if(cardirection==1)
		{
			cardirec='E';
			kprintf("Action: Leave        Car Number : %ld\t Source: %c\t destination: %c\n", carnumber,cardirec,turnPossibility);
		}
		if(cardirection==2)
		{
			cardirec='N';
			kprintf("Action: Leave        Car Number : %ld\t Source: %c\t destination: %c\n", carnumber,cardirec,turnPossibility);
		}
		if(cardirection==3)
		{
			cardirec='W';
			kprintf("Action: Leave        Car Number : %ld\t Source: %c\t destination: %c\n", carnumber,cardirec,turnPossibility);
		}
		break;
		}
	}
}




/*
 * * createcars()
 * *
 * * Arguments:
 * *      int nargs: unused.
 * *      char ** args: unused.
 * *
 * * Returns:
 * *      0 on success.
 * *
 * * Notes:
 * *      Driver code to start up the approachintersection() threads.  You are
 * *      free to modiy this code as necessary for your solution.
 * */

int
createcars(int nargs,
          char ** args)
{
       int index, error;
       //Create locks for the four resources in the intersection 	
       resources[0] = lock_create("Resource 0");
       resources[1] = lock_create("Resource 1");
       resources[2] = lock_create("Resource 2");
       resources[3] = lock_create("Resource 3");
       /*
 *         * Avoid unused variable warnings.
 *                 */

       (void) nargs;
       (void) args;

       /*
 *         * Start NCARS approachintersection() threads.
 *                 */
       if( resources[0] == NULL || resources[1]==NULL || resources[1] == NULL || resources[1]==NULL)
               panic("Stoplight Initialization Failure in lock creation");

       kprintf("\nSource : N->North,S->South,E->East,W->West ; destination : S-> Straight, L->Left, R->Right\n\n ");
       for (index = 0; index < NCARS; index++) {

               error = thread_fork("approachintersection thread",
                                   NULL,
                                   index,
                                   approachintersection,
                                   NULL
                                   );

               /*
 *                 * panic() on error.
 *                                 */

               if (error) {
                       
                       panic("approachintersection: thread_fork failed: %s\n",
                             strerror(error)
                             );
               }

       }

       while(m<(NCARS)); //Wait for the child thread to terminate
       kprintf("\nSuccess!! %d cars passed the intersection.\n\n",(m));
       m=0;
      // Destroy all the created locks
       lock_destroy(resources[0]);
       lock_destroy(resources[1]);
       lock_destroy(resources[2]);
       lock_destroy(resources[3]);
       return 0;
}









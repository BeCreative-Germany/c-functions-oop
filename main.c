/* 
 * File:   main.c
 * Author: dustin deus
 * 
 *  ### WORKING WITH FUNCTION POINTERS AND LITTLE BIT OOP IN C ###
 * 
 * During the working with Go I was inspired to create a similiar functionality in C becouse everybody rejects the working 
 * with C and lots of people think C is to low and nobody need it to go down but in my opinion only in programming with C you get the feeling
 * and knowledge how a language internally works. 
 * I´m not a professional C programmer but I like it and I will do everything to become one.
 * 
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>


typedef struct base_struct Base_struct;
typedef struct sub_struct Sub_struct;
typedef struct func Func;

//typedef for our function pointer
typedef void (*fnptr_t)(Sub_struct *self,int argsCount,int a, ...);
//function pointer with base signature
typedef void (*fnptr_b)(Sub_struct *self,int argsCount);

//Typedef string
typedef char* String;

//Max length for funcname
typedef const char* FuncName;

//Base
struct base_struct {
    int id;
};
//Sub
struct sub_struct {
    
    Base_struct super;
    String name;
    fnptr_t (*callVariable)(Func *self,FuncName name);
    fnptr_b (*callStatic)(Func *self,FuncName name);
    Func *functions[10];
    
};

struct func {
    
    union { //For memory optimation (A function has always one signature) 
   
        fnptr_t f;
        fnptr_b b;
             
    };
    
    FuncName name;
    int *args[];
      
};

/*
 This function return all overload arguments of a function with a specific type as an pointer array.This method implies right memory alignment.
It was my intention to use this method becouse to know how it works. For critical systems please use <a href="http://linux.die.net/man/3/va_arg"
 */
int* ArgumentIntFactory(int firstArg,int argsCount) {
    
    int *p; //Pointer to first Argument
    int arguments[argsCount]; //Array for storing the arguments
    int arrayC = argsCount; // For reverse saving the argumets into the array
    int length = argsCount; //Arguments Length
    
    if (argsCount)
    {   
          
        p = &firstArg;
        
        //The parameters of the function are located in a reverse order
        /*
         
         * E.g
         myFunction(2,3,4,5)
         
         With the first argument you have the opportunity to get the memory street of the function arguments on the stack.
         This street begins in *Memory* with the last argument of *myFunction* so the order to read is reverse.
         
         (int)(*p+0) = 5
         (int)(*p+1) = 4
         (int)(*p+2) = 3
         (int)(*p+3) = 2
         
         */
        while(argsCount--) {
            
            printf("Arg-Adresse:%p \n",(p+argsCount));
            int a = (int)(*p+argsCount);
            arguments[--arrayC] = a;
            
             
        
        }
        
       

    }
    
    //Allocate Memory
    void *aa = malloc((length) * sizeof(int));
    
    //If didn´t work to allocate the memoryif did 
    if(aa == NULL){
        
        free(aa);
        return NULL;
    }
    
    
    //Copy elements to the allocated memory
    int i = 0;
    for (i ; i < length; i++){
        
        int *pI = (int*)(aa);
        (pI)[i] = arguments[i];
        
    }
    return aa;

}
/*
 
 Call a method by name with variable int arguments
 
 */
fnptr_t callVariable(Sub_struct *self,String name) {
    
    int f = 0;
    for(f; f < sizeof(self->functions) / sizeof(Func); f++){
        
        int nameLength = (int)((strlen(self->functions[f]->name)+1) * sizeof(char)); //calculate size of the function name
        
        if(strncmp(self->functions[f]->name, name,nameLength) == 0){
            
            printf("Called: %s\n",self->functions[f]->name);
            return (fnptr_t)self->functions[f]->f;
        }
    }
    return NULL;
    
}
/*
 
 Call a method by name with just the sub_struct instance
 
 */
fnptr_b callStatic(Sub_struct *self,String name) {
    
    int f = 0;
    for(f; f < sizeof(self->functions) / sizeof(Func); f++){
        
        int nameLength = (int)((strlen(self->functions[f]->name)+1) * sizeof(char)); //calculate size of the function name
        
        if(strncmp(self->functions[f]->name, name,nameLength) == 0){
            
            printf("Called: %s\n",self->functions[f]->name);
            return (fnptr_b)self->functions[f]->b;
        }
    }
    return NULL;
    
}
/*
 First test function with the standard signature for member methods without arguments
 
*/
void Constructor(Sub_struct *self){
    
    printf("Hey I´ḿ the Constructor! My ID is: %d\n",self->super.id);
}
 /*
 First test function with the standard signature for variable int arguments
 
 */
void Hello(Sub_struct *self,int argsCount, int a){
    
  
    int *args =ArgumentIntFactory(a,argsCount);
   
    int i = 0;
    for (i ; i < argsCount; i++){
        
        
        printf("Argument: %d\n",(args)[i]);
        
    }
    
    if(args != NULL)
    free(args);
    
    printf("Hello %s\n", self->name); //Hello Dustin
    
    
    
}
/*
 
 Second test function with the standard signature for variable int arguments
 
 */
void GoodBye(Sub_struct *self,int argsCount, int a){
    
    printf("Good Bye %s\n", self->name); //print Good Bye Dustin
}

/*
 * 
 */
int main(int argc, char** argv) {
    
//Create our first function
Func f1;
f1.f = (fnptr_t)&Hello;
f1.name = "Hello";

//Create our second function
Func f2;
f2.f = (fnptr_t)&GoodBye;
f2.name = "GoodBye";

Func f3;
f3.b = (fnptr_b)&Constructor;
f3.name = "init";


//Create the sub struct which contain the base and our member function
Sub_struct s = {

    .name = "Dustin",
    .callVariable = &callVariable,
    .callStatic = &callStatic,
    .functions = { &f1, &f2, &f3 }


};

//Call method by Index with variable int arguments
//s.functions[0]->f(&s,4, 2,3,4,5);


//----------------------------------------------------------------------
//#Method1#
//Call method by type def and pointer declaration with just sub instance
fnptr_b *fpsI =  s.callStatic(&s,"init");
//pointer declaration
void (*funcInit)(Sub_struct *self) = fpsI;
//Call method
funcInit(&s);
//----------------------------------------------------------------------
//#Method2#
//Call method by type def and pointer declaration with variable int arguments
fnptr_t *fp =  s.callVariable(&s,"Hello");
//pointer declaration
void (*funcc)(Sub_struct *self,int argsCount,int a, ...) = fp;
//Call method
funcc(&s,4, 2,3,4,5);
//----------------------------------------------------------------------
//#Method3#
//Call method by type def and pointer declaration with just sub instance
fnptr_t *fps =  s.callVariable(&s,"GoodBye");
//pointer declaration
void (*funcS)(Sub_struct *self) = fps;
//Call method
funcS(&s);





//Working with the Base
((Base_struct*)&s)->id = 43;

//or
/*Base_struct *b = &s.super; 
b->value = 43;*/



printf("%s\n",s.name); //print 13
printf("Größe des SubStruct %d\n",sizeof(Sub_struct));

printf("%p\n",&s);       //Points to super "base_struct"
printf("%p\n",&s.super); //Points to super "base_struct"


printf("%d",s.super.id);

return (EXIT_SUCCESS);
}


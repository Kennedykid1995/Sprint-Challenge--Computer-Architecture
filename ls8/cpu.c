#include "cpu.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#define DATA_LEN 6

unsigned char read_ram(struct cpu *cpu, unsigned char address)
  {
    return cpu->ram[address]; 
  }

  void write_ram(struct cpu *cpu, unsigned char address, unsigned char val)
  {
    cpu->ram[address] = val;
  }

/**
 * Load the binary bytes from a .ls8 source file into a RAM array
 */

void cpu_load(struct cpu *cpu, char *filename)
{
  FILE *fp = fopen(filename,"r");
  char line[1024];
  unsigned char address = 0; 

  if(fp == NULL){
    printf("Error opening file\n");
    exit(1);
  }

  // int mem_index = 0;
  while(fgets(line, sizeof line, fp) != NULL){
    // memory[mem_index] = strtoul(line, NULL, 10);
    // mem_index++;
    unsigned char b;
    b = strtoul(line, NULL, 2);
    write_ram(cpu, address++, b);
  }
  fclose(fp);
  // TODO: Replace this with something less hard-coded
}

/*
 * ALU
//  */
void alu(struct cpu *cpu, enum alu_op op, unsigned char regA, unsigned char regB)
{
  switch(op){
    case ALU_MUL:
      cpu-> registers[regA] *= cpu-> registers[regB];
      break;

    case ALU_ADD:
      cpu->registers[regA] += cpu->registers[regB];
      break; 
  }
}

/**
 * Run the CPU
 */
void cpu_run(struct cpu *cpu)
{
  int running = 1; // True until we get a HLT instruction


  while (running) {
    // TODO
    // 1. Get the value of the current instruction (in address PC).
    // 2. switch() over it to decide on a course of action.
    // 3. Do whatever the instruction should do according to the spec.
    // 4. Move the PC to the next instruction.
    unsigned char operandA = read_ram(cpu, cpu->PC + 1);
    unsigned char operandB = read_ram(cpu, cpu->PC + 2);
    unsigned char i =  read_ram(cpu, cpu->PC);
    switch(i)
    {
      case LDI:
      // printf("working-LDI");
        cpu->registers[operandA] = operandB;
        cpu->PC += 3;
        break;

      case POP:
      // printf("working-POP");
        cpu->registers[operandA] = read_ram(cpu, cpu->registers[7]);
        cpu->registers[7]++;
        cpu->PC += 2;  
        break;

      case PUSH:
      // printf("working-PUSH");
        cpu->registers[7]--;
        write_ram(cpu, cpu->registers[7], cpu->registers[operandA]);
        cpu->PC += 2;
        break;

      case CALL:
      // printf("working-CALL");
    // 1. The address of the ***instruction*** _directly after_ `CALL` is
    //    pushed onto the stack. This allows us to return to where we left off when the subroutine finishes executing.
    // 2. The PC is set to the address stored in the given register. We jump to that location in RAM and execute the 
          // first instruction in the subroutine. The PC can move forward or backwards from its current location.
        cpu->registers[7]--;
        write_ram(cpu, cpu->registers[7], cpu->PC + 2);
        cpu->PC = cpu->registers[operandA] + 1;
        break; 
        
      case RET:
      // printf("working-RET");
      // Return from subroutine.
      // Pop the value from the top of the stack and store it in the `PC`.
        cpu->PC = read_ram(cpu, cpu->registers[7]);
        cpu->registers[7]++; 
        break;

      case PRN:
      // printf("working-PRN");
        printf("%d\n", cpu->registers[operandA]);
        cpu->PC += 2;
        break;

      case MUL:
      // printf("working-MUL");
        alu(cpu, ALU_MUL, operandA, operandB); 
        cpu->PC += 3;
        break;

      case ADD:
      // printf("working-ADD");
        alu(cpu, ALU_ADD, operandA, operandB);
        cpu->PC += 3;
        break; 

      case CMP: 
      //Compare the values in two registers.

      // * If they are equal, set the Equal `E` flag to 1, otherwise set it to 0.
      // * If registerA is less than registerB, set the Less-than `L` flag to 1,
      //   otherwise set it to 0.
      // * If registerA is greater than registerB, set the Greater-than `G` flag
      //   to 1, otherwise set it to 0.
          //Each of the occurences has to have a true or false outcome.
          //true = 1;
          //false = 0;
        if(cpu->registers[operandA] == cpu->registers[operandB]){
          cpu->E_FLAG = 1;
          printf("%d  == \n", cpu->registers[operandA]);
        }else{
          cpu->E_FLAG = 0; 
        }
        if (cpu->registers[operandA] > cpu->registers[operandB]){
          printf("%d  > \n", cpu->registers[operandA]);
          cpu->GREATER_FLAG = 1;
        }else{
           printf("%d  > 0 \n", cpu->registers[operandA]);
          cpu->GREATER_FLAG = 0; 
        }
        if (cpu->registers[operandA] < cpu->registers[operandB]){
          printf("%d <\n", cpu->registers[operandA]);
          cpu->LESS_FLAG = 1;
        }else{
          cpu->LESS_FLAG = 0;
        }
        cpu->PC += 3; 
        break;
      
      case JMP:
      // Jump to the address stored in the given register.
      // (difference between CALL(Jump to location in RAM)
      // and JMP(jump to address stored in given Register)
      // **Set the `PC` to the address stored in the given register.
        cpu->registers[7] = cpu->registers[7 - 1];
        cpu->PC = cpu->registers[operandA]; 
        break; 
      
      case JEQ:
      // If `equal` flag is set (true), 
      // jump to the address stored in the given register.
      //E flag must be equal to 1
      //then PC will equal the stored address
      //register - operandA
      if(cpu->E_FLAG == 1){
        cpu->PC = cpu->registers[operandA];
        printf("%d JEQ\n", cpu->registers[operandA]);
      }else{
        cpu->PC += 2; 
      }
      break; 

      case JNE:
        // If `E` flag is clear (false, 0),
        // jump to the address stored in the given register.
        //if E is equal to 0
        //then PC will equal the address at the given register
        //register - operandA

      if(cpu->E_FLAG == 0){
        cpu->PC = cpu->registers[operandA];
         printf("%d  JNE\n", cpu->registers[operandA]);
      }else{
        cpu->PC += 2; 
      }
      break; 

      case HLT: 
      printf("working-HLT");
        running = 0;
        cpu->PC++;
        break; 

      //avoid comment lines  
      default:
        cpu->PC++;
    }
  }
}

/**
 * Initialize a CPU struct
 */
void cpu_init(struct cpu *cpu)
{
  // TODO: Initialize the PC and other special registers
  cpu->PC = 0;
  // TODO: Zero registers and RAM
  memset(cpu->ram, 0, sizeof(cpu->ram));
  memset(cpu->registers, 0, sizeof(cpu->registers));
  cpu->registers[7] = 0xF4;
}


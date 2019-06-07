#include "execute_data_trans.h"
#include "execute.h"
#include <stdio.h>

void exec_data_trans_instr(Emulator *emulator, Data_Trans_Instr *instr)
{
  if (!is_cond_true(emulator, instr->cond))
  {
    return;
  }

  uint32_t offset = 0;
  if (instr->i) /* Offset is shifted register */
  {
    uint32_t carry = 0;
    offset = compute_offset_from_reg(emulator, instr->offset, &carry);
  }
  else /* Offset is unsigned 12 bit immediate offset */
  {
    offset = instr->offset;
  }

  uint32_t base = emulator->regs[instr->rn];

  if (instr->p)
  {
    base = instr->u ? base + offset : base - offset;
  }

  if (base > MEM_SIZE)
  {
    if (base == 0x20200008)
    {
      printf("One GPIO pin from 20 to 29 has been accessed\n");
    }
    else if (base == 0x20200004)
    {
      printf("One GPIO pin from 10 to 19 has been accessed\n");
    }
    else if (base == 0x20200000)
    {
      printf("One GPIO pin from 0 to 9 has been accessed\n");
    }
    else if (base == 0x2020001C)
    {
      printf("PIN ON\n");
    }
    else if (base == 0x20200028)
    {
      printf("PIN OFF\n");
    }
    else
    {
      printf("Error: Out of bounds memory access at address 0x%08x\n", base);
    }
    return;
  }

  uint32_t dst_src = emulator->regs[instr->rd];

  if (instr->l) /* Load from memory */
  {
    emulator->regs[instr->rd] = get_mem(emulator, base);
  }
  else /* Store to memory */
  {
    set_mem(emulator, base, dst_src);
  }

  /* Only support post indexing */
  if (!instr->p)
  {
    emulator->regs[instr->rn] = instr->u ? base + offset : base - offset;
  }

  /* Save the new values into regs */
  // emulator->regs[instr->rn] = base;
  // emulator->regs[instr->rd] = dst_src;
}

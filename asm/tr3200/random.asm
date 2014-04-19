; tests the embedded RNG device

  .ORG 0x100000       ; This a ROM image

  MOV %R8, 0x789

  STORE 0x11E040, %R8

  LOAD %R1, 0x11E040
  LOAD %R2, 0x11E040
  LOAD %R3, 0x11E040

  STORE 0x11E040, %R8

  LOADW %R5, 0x11E040
  LOADW %R6, 0x11E040
  LOADW %R7, 0x11E040

  STORE 0x11E040, %R8

  LOADB %R9, 0x11E040
  LOADB %R10, 0x11E040
  LOADB %Y, 0x11E040

end:
  SLEEP
  JMP end ; Looks that WaveAsm not like to have a label like an instruction


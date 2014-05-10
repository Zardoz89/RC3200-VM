/**
 * \brief       Macros used by DCPU16N implementation
 * \file        DCPU16N_macros.hpp
 * \copyright   The MIT License (MIT)
 *
 */

#ifndef __DCPU16N_MACROS_HPP__
#define __DCPU16N_MACROS_HPP__

#define DCPU16N_PHASE_OPFETCH 0
#define DCPU16N_PHASE_NWAFETCH 1
#define DCPU16N_PHASE_NWBFETCH 2
#define DCPU16N_PHASE_UAREAD 3
#define DCPU16N_PHASE_ACUFETCH 4
#define DCPU16N_PHASE_UBREAD 5
#define DCPU16N_PHASE_BCUFETCH 6
#define DCPU16N_PHASE_EXEC 7
#define DCPU16N_PHASE_UAWRITE 8
#define DCPU16N_PHASE_ACUWRITE 9
#define DCPU16N_PHASE_UBWRITE 10
#define DCPU16N_PHASE_BCUWRITE 11
#define DCPU16N_PHASE_EXECW 12
#define DCPU16N_PHASE_SLEEP 13
#define DCPU16N_PHASE_EXECSKIP 14
#define DCPU16N_PHASE_EXECJMP 15
#define DCPU16N_PHASE_EXECRFI 16
#define DCPU16N_PHASE_MARKSKIP 17
#define DCPU16N_PHASE_INTERRUPT 18

#endif // ifndef __DCPU16N_MACROS_HPP__

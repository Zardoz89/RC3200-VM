#pragma once
/**
* Trillek Virtual Computer - M5FDD.hpp
* Mackapar 5.25" Floppy Drive
*/

#include "Types.hpp"
#include "VComputer.hpp"

#include "Disk.hpp"

#include <string>
#include <iostream>
#include <fstream>

namespace vm {
    namespace dev {
        namespace m5fdd {

            /**
            * M35 Floppy Drive commands
            */
            enum class COMMANDS : uint16_t {
                SET_INTERRUPT = 0x0,
                READ_SECTOR = 0x1,
                WRITE_SECTOR = 0x2,
                QUERY_MEDIA = 0x3,
            };

            /**
            * M35 Floppy Drive status codes
            */
            enum class STATE_CODES : uint16_t {
                NO_MEDIA = 0,   /// There's no floppy in the drive
                READY = 1,      /// The drive is ready to accept commands
                READY_WP = 2,   /// Same as ready, but the floppy is Write Protected
                BUSY = 3,       /// The drive is busy either reading or writing a sector
            };

            /**
            * M35 Floppy Device error codes
            * This is a superset of Disk ERROR enum
            */
            enum class ERROR_CODES : uint16_t {
                NONE = 0,       /// No error since the last poll
                BUSY = 1,       /// Drive is busy performing a action
                NO_MEDIA = 2,   /// Attempted to read or write without a floppy
                PROTECTED = 3,  /// Attempted to write to a protected floppy
                EJECT = 4,      /// The floppy was ejected while was reading/writing
                BAD_SECTOR = 5, /// The requested sector is broken, the data on it is lost

                BROKEN = 0xFFFF /// There's been some major software/hardware problem.
                /// Try to do a hard reset the device.
            };

            /**
            * Mackapar 3,5" floppy drive
            */
            class M5FDD : public IDevice {
            public:
                M5FDD();
                virtual ~M5FDD();

                /*!
                * Resets device internal state
                * Called by VComputer
                */
                virtual void Reset();

                /**
                * Sends (writes to CMD register) a command to the device
                * @param cmd Command value to send
                */
                virtual void SendCMD(word_t cmd);

                virtual void A(word_t val) { a = val; }
                virtual void B(word_t val) { b = val; }
                virtual void C(word_t val) { c = val; }

                virtual word_t A() { return a; }
                virtual word_t B() { return b; }
                virtual word_t C() { return c; }
                virtual word_t D() { return static_cast<word_t>(state); }
                virtual word_t E() { return static_cast<word_t>(error); }

                /**
                * Device Type
                */
                virtual byte_t DevType() const {
                    return 0x08; // Mass Storage Device
                }

                /**
                * Device SubType
                */
                virtual byte_t DevSubType() const {
                    return 0x01; // Floppy Drive
                }

                /**
                * Device ID
                */
                virtual byte_t DevID() const {
                    return 0x01; // Mackapar 5.25" Floppy Drive
                }

                /**
                * Device Vendor ID
                */
                virtual dword_t DevVendorID() const {
                    return 0x1EB37E91; // Mackapar Media
                }

                /*!
                * Return if the device does something each Device Clock cycle.
                * Few devices really need to do this, so IDevice implementation
                * returns false.
                */
                virtual bool IsSyncDev() const {
                    return true;
                }

                /*!
                * Executes N Device clock cycles.
                *
                * Here resides the code that is executed every Device Clock tick.
                * IDevice implementation does nothing.
                * \param n Number of clock cycles to be executed
                * \param delta Number milliseconds since the last call
                */
                virtual void Tick(unsigned n = 1, const double delta = 0);

                /*!
                * Checks if the device is trying to generate an interrupt
                *
                * IDevice implementation does nothing.
                * \param[out] msg The interrupt message will be written here
                * \return True if is generating a new interrupt
                */
                virtual bool DoesInterrupt(word_t& msg);

                /*!
                * Informs to the device that his generated interrupt was accepted by the CPU
                *
                * IDevice implementation does nothing.
                */
                virtual void IACK();

                /*!
                * Writes a copy of Device state in a chunk of memory pointer by ptr.
                * \param[out] ptr Pointer were to write
                * \param[in,out] size Size of the chunk of memory were can write. If is
                * successful, it will be set to the size of the write data.
                */
                virtual void GetState(void* ptr, std::size_t& size) const {}

                /*!
                * Sets the Device state.
                * \param ptr[in] Pointer were read the state information
                * \param size Size of the chunk of memory were will read.
                * \return True if can read the State data from the pointer.
                */
                virtual bool SetState(const void* ptr, std::size_t size) { return true; }

                //----------------------------------------------------

                /**
                * @brief Inserts a floppy in the unit
                * If there is a floppy disk previously inserted, this is ejected
                * @param floppy Floppy disk
                */
                void insertFloppy(std::shared_ptr<vm::dev::disk::Disk> floppy);

                /**
                * @brief Ejects the floppy actually inserted if is there one
                */
                void ejectFloppy();

            private:

                /**
                * Moves the head to the desired track
                */
                void setSector(uint16_t sector);

                std::shared_ptr<vm::dev::disk::Disk> floppy;     /// Floppy inserted
                std::vector<byte_t> sectorBuffer; // buffer of sector being accessed
                STATE_CODES state;    /// Floppy drive actual status
                ERROR_CODES error;    /// Floppy drive actual error state

                bool busy;            /// Is true while the drive is reading/writing to a sector
                bool writing;         /// is the drive reading or writing from disk?
                unsigned curSector;   /// current absolute sector the head is at
                dword_t dmaLocation;  /// RAM Location for the DMA transfer
                bool dmaAccess;       /// Does the device have access?
                bool performingDMA;   /// Are we waiting for a DMA to complete?

                uint16_t msg;         /// Msg to send if need to trigger a interrupt
                bool pendingInterrupt;/// Must launch a interrupt from device to CPU ?
                dword_t a, b, c, d;   /// Data registers
            };

        } // End of namespace m5ffd
    } // End of namespace dev
} // End of namespace vm

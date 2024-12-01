#ifndef BITVAULT_H
#define BITVAULT_H

#include <Arduino.h>
#include <atomic>

// Primary template for BitVault (non-atomic version)
template<typename T, bool enable_atomic = false>
class BitVault {
  private:
    T bits = 0;  // Use a regular variable to hold bits

  public:
    // Retrieves the value of the bit at the specified 'position'
    bool get(uint8_t position) const {
        return (bits & (static_cast<T>(1) << position)) != 0;
    }

    // Sets or clears the bit at the specified 'position' based on the 'value' parameter
    void set(uint8_t position, bool value) {
        T bitmask = static_cast<T>(1) << position;  // Create a bitmask for the specific bit

        if (value) {
            bits |= bitmask;  // Set the bit at 'position'
        } else {
            bits &= ~bitmask; // Clear the bit at 'position'
        }
    }

    // Clears all the bits by resetting 'bits' to 0
    void clear_all() {
        bits = 0;  // Direct assignment to clear all bits
    }
};

// Partial specialization for BitVault (atomic version)
template<typename T>
class BitVault<T, true> {
  private:
    std::atomic<T> bits = 0;

  public:
    // Retrieves the value of the bit at the specified 'position'
    bool get(uint8_t position) const {
        return (bits.load(std::memory_order_acquire) & (static_cast<T>(1) << position)) != 0;
    }

    // Sets or clears the bit at the specified 'position' based on the 'value' parameter
    void set(uint8_t position, bool value) {
        T bitmask = static_cast<T>(1) << position;  // Create a bitmask for the specific bit

        if (value) {
            bits.fetch_or(bitmask, std::memory_order_release);  // Set the bit at 'position'
        } else {
            bits.fetch_and(~bitmask, std::memory_order_release); // Clear the bit at 'position'
        }
    }

    // Clears all the bits by resetting 'bits' to 0
    void clear_all() {
        bits.store(0, std::memory_order_release);  // Clear all bits using atomic store
    }
};

#endif

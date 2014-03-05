//
//  This file is part of the Patmos Simulator.
//  The Patmos Simulator is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  The Patmos Simulator is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with the Patmos Simulator. If not, see <http://www.gnu.org/licenses/>.
//
//
// Provide information to map addresses to symbols.
//

#ifndef PATMOS_SYMBOL_H
#define PATMOS_SYMBOL_H

#include "basic-types.h"

#include <string>

#include <boost/tuple/tuple.hpp>

namespace patmos
{
  class symbol_map_t;
  
  struct reloc_info_t {
    std::string SymA;
    // If not empty, the result of the relocation is A - B.
    std::string SymB;
    // Additional constant added to the symbol values
    word_t Addend;
    // Size in bits of the relocation
    unsigned Size;
    // Offset in bits of the value
    unsigned Offset;
    // Shift the value of symbols to the right by the given amout of bits
    unsigned Shift;
    // Relative to PC?
    bool Relative;
    
    reloc_info_t() : Addend(0), Size(0), Offset(0), Shift(0), Relative(false) {}
    
    void set_format(unsigned size, unsigned offset = 0, unsigned shift = 0, 
                    bool relative = false)
    {
      Size = size; 
      Offset = offset; 
      Shift = shift; 
      Relative = relative; 
    }
    
    void set_PCRel_format(unsigned size) 
    {
      set_format(size, 0, 2, true);
    }
    
    void set_word_format(unsigned size)
    {
      set_format(size, 0, 2, false);
    }
    
    bool get_value(symbol_map_t &symbols, word_t &value, word_t PC = 0) const;
  };
  

  
  /// Symbol information, start address, size
  struct symbol_info_t
  {
  public:
    /// Address of the symbol.
    word_t Address;

    /// Size in bytes occupied by the symbol.
    word_t Size;

    /// If the symbol is a function
    bool IsFunc;

    /// Name of the symbol.
    std::string Name;

    /// construct a symbol info record.
    symbol_info_t(word_t address, word_t size, bool isfunc,
        const std::string &name) :
      Address(address), Size(size), IsFunc(isfunc), Name(name)
    {
    }

  };

  /// Provide a mapping from addresses to symbol names.
  class symbol_map_t
  {
  private:

    /// List of symbol names.
    typedef std::vector<symbol_info_t> symbols_t;

    /// List of known symbols, The list is sorted after initialization.
    symbols_t Symbols;

    /// Flag indicating whether the symbol list is sorted.
    bool Is_sorted;
  public:
    /// Construct a symbol map.
    symbol_map_t() : Is_sorted(true)
    {
    }

    /// Sort the symbol list.
    void sort();

    /// Append an entry to the symbol map.
    /// the entry is simply appended at the end of the map, i.e., the sort
    /// function has to be invoked once all symbols were added to the map.
    /// @param symbol The symbol to add.
    void add(const symbol_info_t &symbol);

    /// Check if the map contains a symbol for an address.
    /// @param address The address for which the symbol should be searched.
    /// @return True if the map contains at least one symbol for that address.
    bool contains(word_t address) const;

    /// Check if the map contains a symbol.
    /// @param symbol the name of the symbol.
    /// @return True if the map contains at least one symbol with that name.
    bool contains(std::string symbol) const;
    
    /// Check if a symbol at a given address covers the given address, i.e,
    /// if there is a symbol at address @symbol and @addess is in 
    /// [@symbol,@symbol+@symbol.size)
    /// @param symbol an address of a symbol
    /// @param address the address to check
    /// @return True if the a symbol at @symbol has a size attribute and covers @address
    bool covers(word_t symbol, word_t address) const;
    
    /// Find a symbol given a specific address.
    /// \see print
    /// @param address The address for which symbol information should be
    /// retrieved.
    /// @return A string representing symbol information.
    std::string find(word_t address) const;

    /// Find an address by symbol name.
    /// @return -1 if not found, the address of the symbol otherwise.
    word_t find(std::string symbol) const;
    
    /// Print symbol information given a specific address to a stream.
    /// The symbol table is searched for the symbol covering the address and the
    /// name of the symbol is returned. In case labels of basic blocks within
    /// functions are available the name of the enclosing function and the name
    /// of the basic block are concatenated, e.f., for basic block 'bar' in
    /// function 'foo' 'foo:bar' is returned.
    /// @param os An output stream.
    /// @param address The address for which symbol information should be
    /// retrieved.
    /// @param func_only If true, print only symbols with function attribute
    /// @return A string representing symbol information.
    std::ostream &print(std::ostream &os, word_t address, bool func_only = false) const;
  };
}

#endif // PATMOS_SYMBOL_H



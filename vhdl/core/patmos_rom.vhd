--
--	patmos_rom.vhd
--
--	generic VHDL version of ROM
--
--		DONT edit this file!
--		generated by patmos.asm.Bin2Vhdl
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity patmos_rom is
port (
    address : in unsigned(7 downto 0);
    q : out unsigned(31 downto 0)
);
end patmos_rom;

architecture rtl of patmos_rom is

begin

process(address) begin

case address is
    when "00000000" => q <= "00000000001000010000000001000000";
    when "00000001" => q <= "00000000000000100000000000000010";
    when "00000010" => q <= "00000010100101000101000110000000";
    when "00000011" => q <= "00000000000000000000000000000001";
    when "00000100" => q <= "00000010000101101010000010000111";
    when "00000101" => q <= "00000111110000000001010110000100";
    when "00000110" => q <= "00000000000000000000000000000001";
    when "00000111" => q <= "00000000000000000000000000000001";
    when "00001000" => q <= "00000010100111100101000110000001";
    when "00001001" => q <= "00000010100111100101000110000001";
    when "00001010" => q <= "00000000001000110001000000011000";
    when "00001011" => q <= "00000010000111101111100010000011";
    when "00001100" => q <= "00000010100101000101000110000000";
    when "00001101" => q <= "00000000000000000000000000000001";
    when "00001110" => q <= "00000010000101101010000010000111";
    when "00001111" => q <= "00000111110000000001010110000100";
    when "00010000" => q <= "00000000000000000000000000000001";
    when "00010001" => q <= "00000000000000000000000000000001";
    when "00010010" => q <= "00000010101001000101000110000001";
    when "00010011" => q <= "00000010101001000101000110000001";
    when "00010100" => q <= "00000000001001110011000000010000";
    when "00010101" => q <= "00000010001001010010100110000011";
    when "00010110" => q <= "00000010000111101111100100000110";
    when "00010111" => q <= "00000010100101000101000110000000";
    when "00011000" => q <= "00000000000000000000000000000001";
    when "00011001" => q <= "00000010000101101010000010000111";
    when "00011010" => q <= "00000111110000000001010110000100";
    when "00011011" => q <= "00000000000000000000000000000001";
    when "00011100" => q <= "00000000000000000000000000000001";
    when "00011101" => q <= "00000010101010000101000110000001";
    when "00011110" => q <= "00000010101010000101000110000001";
    when "00011111" => q <= "00000000001010110101000000001000";
    when "00100000" => q <= "00000010001010010100101010000011";
    when "00100001" => q <= "00000010000111101111101000000110";
    when "00100010" => q <= "00000010100101000101000110000000";
    when "00100011" => q <= "00000000000000000000000000000001";
    when "00100100" => q <= "00000010000101101010000010000111";
    when "00100101" => q <= "00000111110000000001010110000100";
    when "00100110" => q <= "00000000000000000000000000000001";
    when "00100111" => q <= "00000000000000000000000000000001";
    when "00101000" => q <= "00000010101011000101000110000001";
    when "00101001" => q <= "00000010101011000101000110000001";
    when "00101010" => q <= "00000000000000000000000000000001";
    when "00101011" => q <= "00000010000111101111101100000110";
    when "00101100" => q <= "00000000000011100111000111111111";
    when "00101101" => q <= "00000010110001100111011110000001";
    

    when others => q <= "00000000000000000000000000000000";
end case;
end process;

end rtl;
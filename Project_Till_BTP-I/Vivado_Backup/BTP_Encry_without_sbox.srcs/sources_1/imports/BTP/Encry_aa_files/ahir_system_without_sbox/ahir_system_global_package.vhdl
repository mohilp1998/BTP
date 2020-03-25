-- VHDL global package produced by vc2vhdl from virtual circuit (vc) description 
library ieee;
use ieee.std_logic_1164.all;
package ahir_system_global_package is -- 
  constant check_base_address : std_logic_vector(0 downto 0) := "0";
  constant decry_counter : std_logic_vector(127 downto 0) := "01010100011101110110111100100000010011110110111001100101001000000100111001101001011011100110010100100000010101000111011101101111";
  constant encry_counter : std_logic_vector(127 downto 0) := "01010100011101110110111100100000010011110110111001100101001000000100111001101001011011100110010100100000010101000111011101101111";
  constant key_0 : std_logic_vector(31 downto 0) := "01010100011010000110000101110100";
  constant key_1 : std_logic_vector(31 downto 0) := "01110011001000000110110101111001";
  constant key_2 : std_logic_vector(31 downto 0) := "00100000010010110111010101101110";
  constant key_3 : std_logic_vector(31 downto 0) := "01100111001000000100011001110101";
  constant one : std_logic_vector(7 downto 0) := "00000001";
  constant one_1bit : std_logic_vector(0 downto 0) := "1";
  constant rcon_base_address : std_logic_vector(3 downto 0) := "0000";
  constant sbox_base_address : std_logic_vector(7 downto 0) := "00000000";
  constant zero : std_logic_vector(7 downto 0) := "00000000";
  constant zero24 : std_logic_vector(23 downto 0) := "000000000000000000000000";
  constant zero_1bit : std_logic_vector(0 downto 0) := "0";
  -- 
end package ahir_system_global_package;

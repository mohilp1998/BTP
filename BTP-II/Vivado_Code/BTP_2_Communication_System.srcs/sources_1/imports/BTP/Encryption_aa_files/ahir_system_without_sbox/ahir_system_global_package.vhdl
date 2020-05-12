-- VHDL global package produced by vc2vhdl from virtual circuit (vc) description 
library ieee;
use ieee.std_logic_1164.all;
package ahir_system_global_package is -- 
  constant check_base_address : std_logic_vector(0 downto 0) := "0";
  constant one : std_logic_vector(7 downto 0) := "00000001";
  constant one_128 : std_logic_vector(127 downto 0) := "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001";
  constant one_1bit : std_logic_vector(0 downto 0) := "1";
  constant rcon_base_address : std_logic_vector(3 downto 0) := "0000";
  constant sbox_base_address : std_logic_vector(7 downto 0) := "00000000";
  constant zero : std_logic_vector(7 downto 0) := "00000000";
  constant zero24 : std_logic_vector(23 downto 0) := "000000000000000000000000";
  constant zero_1bit : std_logic_vector(0 downto 0) := "0";
  -- 
end package ahir_system_global_package;

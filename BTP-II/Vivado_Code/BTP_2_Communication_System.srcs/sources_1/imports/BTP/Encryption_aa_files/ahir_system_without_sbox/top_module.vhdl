library std;
use std.standard.all;
library ieee;
use ieee.std_logic_1164.all;

library work;
use work.ahir_system_global_package.all;

library work;
use work.uartPackage.all;

entity BTP_encryption is
  port (
  		clk	    : in std_logic;
  		reset	: in std_logic;
--  		TIVA_led    : in std_logic;
  		serIn_TIVA_FPGA		: in std_logic;
  		serOut_FPGA_TIVA	: out std_logic;
  		serIn_COMP_FPGA		: in std_logic;
  		serOut_FPGA_COMP	: out std_logic
	);
end entity ; -- BTP_encryption

architecture arch of BTP_encryption is

	signal uart_read_data_comp, uart_write_data_comp: std_logic_vector(7 downto 0);
	signal uart_read_req_comp, uart_write_req_comp: std_logic_vector(0 downto 0);
	signal uart_read_ack_comp, uart_write_ack_comp: std_logic_vector(0 downto 0);

	signal uart_read_data_tiva, uart_write_data_tiva: std_logic_vector(7 downto 0);
	signal uart_read_req_tiva, uart_write_req_tiva: std_logic_vector(0 downto 0);
	signal uart_read_ack_tiva, uart_write_ack_tiva: std_logic_vector(0 downto 0);
	
	--signal clk_50: std_logic;
	--signal clk_25: std_logic;
	--signal clk_12_5: std_logic;
	
	signal rst: std_logic;
	signal rst_out: std_logic; 

	component ahir_system is  -- system 
	  port (-- 
	    clk : in std_logic;
	    reset : in std_logic;
	    input_d_pipe_write_data: in std_logic_vector(7 downto 0);
	    input_d_pipe_write_req : in std_logic_vector(0 downto 0);
	    input_d_pipe_write_ack : out std_logic_vector(0 downto 0);
	    input_e_pipe_write_data: in std_logic_vector(7 downto 0);
	    input_e_pipe_write_req : in std_logic_vector(0 downto 0);
	    input_e_pipe_write_ack : out std_logic_vector(0 downto 0);
	    out_d_pipe_read_data: out std_logic_vector(7 downto 0);
	    out_d_pipe_read_req : in std_logic_vector(0 downto 0);
	    out_d_pipe_read_ack : out std_logic_vector(0 downto 0);
	    out_e_pipe_read_data: out std_logic_vector(7 downto 0);
	    out_e_pipe_read_req : in std_logic_vector(0 downto 0);
	    out_e_pipe_read_ack : out std_logic_vector(0 downto 0)); -- 
	  -- 
	end component; 
	   
	--component div2 is
	--port (
 --           reset : in std_logic;
 --           clk_in : in std_logic;
 --           clk_out : out std_logic
	--	);
 --   end component;
 component BUFG
        port(I: in STD_LOGIC; O: out STD_LOGIC);
    end component;
begin
    rst <= (not reset);
    reset_signal: BUFG port map (I => rst, O => rst_out);
    
---------------------------------------------------------------------------------------------------
--  UART Instance for TIVA to FPGA     
---------------------------------------------------------------------------------------------------
uart_TIVA_FPGA:uartTop
	generic map(baudFreq => 576, baudLimit => 15049)
	port map(
			-- global signals
			reset => rst_out,
			clk => clk,
			-- uart serial signals
			serIn => serIn_TIVA_FPGA,
			serOut => serOut_FPGA_TIVA,
			-- pipe signals for tx/rx.
			uart_rx_pipe_read_data  =>  uart_read_data_tiva,
 			uart_rx_pipe_read_req   =>  uart_read_req_tiva,
 			uart_rx_pipe_read_ack   =>  uart_read_ack_tiva,
 			uart_tx_pipe_write_data =>  uart_write_data_tiva,
 			uart_tx_pipe_write_req  =>  uart_write_req_tiva,
 			uart_tx_pipe_write_ack  =>  uart_write_ack_tiva
		);

---------------------------------------------------------------------------------------------------
--  UART Instance for COMP to FPGA     
---------------------------------------------------------------------------------------------------
uart_COMP_FPGA:uartTop
	generic map(baudFreq => 576, baudLimit => 15049)
	port map(
			-- global signals
			reset => rst_out,
			clk => clk,
			-- uart serial signals
			serIn => serIn_COMP_FPGA,
			serOut => serOut_FPGA_COMP,
			-- pipe signals for tx/rx.
			uart_rx_pipe_read_data  => uart_read_data_comp,
 			uart_rx_pipe_read_req   => uart_read_req_comp,
 			uart_rx_pipe_read_ack   => uart_read_ack_comp,
 			uart_tx_pipe_write_data => uart_write_data_comp,
 			uart_tx_pipe_write_req  => uart_write_req_comp,
 			uart_tx_pipe_write_ack  => uart_write_ack_comp
		);
---------------------------------------------------------------------------------------------------
--  ahir system Instance     
---------------------------------------------------------------------------------------------------
encry_decry:ahir_system
	port map(-- 
		    
		    clk => clk,
		    reset => rst_out,
		    input_d_pipe_write_data => uart_read_data_comp,
		    input_d_pipe_write_req => uart_read_ack_comp,
		    input_d_pipe_write_ack => uart_read_req_comp,
		    input_e_pipe_write_data => uart_read_data_tiva,
		    input_e_pipe_write_req => uart_read_ack_tiva,
		    input_e_pipe_write_ack => uart_read_req_tiva,
		    out_d_pipe_read_data => uart_write_data_tiva,
		    out_d_pipe_read_req => uart_write_ack_tiva,
		    out_d_pipe_read_ack => uart_write_req_tiva,
		    out_e_pipe_read_data => uart_write_data_comp,
		    out_e_pipe_read_req => uart_write_ack_comp,
		    out_e_pipe_read_ack => uart_write_req_comp
	    );


end architecture ; -- arch
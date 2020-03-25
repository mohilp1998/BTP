-----------------------------------------------------------------------------------------
-- uart top level module  
--
-----------------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

library work;
use work.uartPackage.all;


entity uartTop is
  generic (
         	-- baudFreq = 16 * baudRate / gcd(clkFreq, 16 * baudRate)
		--   For example if clock frequency is 100MHz and 
		--   baud-rate is 9600, then gcd(16*9600,10^8) is
		--   6400.  So Baud-frequency will be 16*9600/6400 = 24
         	--baudFreq  : integer:= 576;
          baudFreq : integer := 576;
         	-- baudLimit = clkFreq / gcd(clkFreq, 16 * baudRate) - baudFreq
		--   For example if clock frequency is 100MHz and
		--   baud-rate is 9600, the gcd is 6400.  So 
		--   baudLimit = 15601.
         	--baudLimit : integer := 15049
          baudLimit : integer := 15049);
  port ( -- global signals
         reset     : in  std_logic;                     -- global reset input
         clk       : in  std_logic;                     -- global clock input
         -- uart serial signals
         serIn     : in  std_logic;                     -- serial data input
         serOut    : out std_logic;                     -- serial data output
         -- pipe signals for tx/rx.
	 uart_rx_pipe_read_data:  out  std_logic_vector (7 downto 0);
	 uart_rx_pipe_read_req:   in   std_logic_vector (0 downto 0);
	 uart_rx_pipe_read_ack:   out  std_logic_vector (0 downto 0);
	 uart_tx_pipe_write_data: in   std_logic_vector (7 downto 0);
	 uart_tx_pipe_write_req:  in   std_logic_vector (0 downto 0);
	 uart_tx_pipe_write_ack:  out  std_logic_vector (0 downto 0));
end uartTop;

architecture Struct of uartTop is

  signal ce16 : std_logic; -- clock enable at bit rate
  signal baudFreq_sig  : std_logic_vector(11 downto 0); -- baud rate setting registers - see header description
  signal baudLimit_sig : std_logic_vector(15 downto 0); -- baud rate setting registers - see header description
  signal baudClk   : std_logic;                    -- 
        
  signal txData    : std_logic_vector(7 downto 0);  -- data byte to transmit
  signal newTxData : std_logic;                     -- asserted to indicate that there is a new data byte for transmission
  signal txBusy    : std_logic;                     -- signs that transmitter is busy
  signal rxData    : std_logic_vector(7 downto 0);  -- data byte received
  signal newRxData : std_logic;                     -- signs that a new byte was received



begin

  baudFreq_sig  <= std_logic_vector(to_unsigned(baudFreq,12));
  baudLimit_sig <= std_logic_vector(to_unsigned(baudLimit,16));

   baseInst: uartTopBase
      port map ( 
			clr       => reset,
           		clk       => clk,
           		serIn     => serIn,    
           		txData    => txData,   
           		newTxData => newTxData,
           		baudFreq  => baudFreq_sig, 
           		baudLimit => baudLimit_sig,
           		serOut    => serOut,   
           		txBusy    => txBusy,   
           		rxData    => rxData,   
           		newRxData => newRxData,
           		baudClk   => baudClk   
		);

   bridgeInst: uartBridge
     port map ( 
	   reset     => reset,
           clk       => clk,
           txData    => txData, 
           newTxData => newTxData,
           txBusy    => txBusy,
           rxData    => rxData,    
           newRxData => newRxData, 
	   uart_rx_pipe_read_data=> uart_rx_pipe_read_data,
	   uart_rx_pipe_read_req=> uart_rx_pipe_read_req,
	   uart_rx_pipe_read_ack=> uart_rx_pipe_read_ack,
	   uart_tx_pipe_write_data=> uart_tx_pipe_write_data,
	   uart_tx_pipe_write_req=> uart_tx_pipe_write_req,
	   uart_tx_pipe_write_ack=> uart_tx_pipe_write_ack
           );

end Struct;

library ieee;
use ieee.std_logic_1164.all;

package uartPackage is

  component baudGen
    port (
      clr       : in  std_logic;
      clk       : in  std_logic;
      baudFreq  : in  std_logic_vector(11 downto 0);
      baudLimit : in  std_logic_vector(15 downto 0);
      ce16      : out std_logic);
  end component;

  component uartTx
    port (
      clr : in  std_logic;
      clk : in  std_logic;
      ce16 : in  std_logic;
      txData : in  std_logic_vector(7 downto 0);
      newTxData : in  std_logic;
      serOut : out  std_logic;
      txBusy : out  std_logic);
  end component;

  component uartRx
    port (
      clr       : in  std_logic;
      clk       : in  std_logic;
      ce16      : in  std_logic;
      serIn     : in  std_logic;
      rxData    : out std_logic_vector(7 downto 0);
      newRxData : out std_logic);
  end component;

  component uartTopBase
    port ( clr       : in  std_logic;
           clk       : in  std_logic;
           serIn     : in  std_logic;
           txData    : in  std_logic_vector(7 downto 0);
           newTxData : in  std_logic;
           baudFreq  : in  std_logic_vector(11 downto 0);
           baudLimit : in  std_logic_vector(15 downto 0);
           serOut    : out std_logic;
           txBusy    : out std_logic;
           rxData    : out std_logic_vector(7 downto 0);
           newRxData : out std_logic;
           baudClk   : out std_logic);
  end component;

  component uartBridge
    port ( reset     : in  std_logic;
           clk       : in  std_logic;
 	   -- from uartTopBase
           txData    : out  std_logic_vector(7 downto 0);
           newTxData : out  std_logic;
           txBusy    : in std_logic;
           rxData    : in std_logic_vector(7 downto 0);
           newRxData : in std_logic;
	   -- Pipe interfaces
	   uart_rx_pipe_read_data:  out  std_logic_vector (7 downto 0);
	   uart_rx_pipe_read_req:   in   std_logic_vector (0 downto 0);
	   uart_rx_pipe_read_ack:   out  std_logic_vector (0 downto 0);
	   uart_tx_pipe_write_data: in   std_logic_vector (7 downto 0);
	   uart_tx_pipe_write_req:  in   std_logic_vector (0 downto 0);
	   uart_tx_pipe_write_ack:  out  std_logic_vector (0 downto 0)
           );
  end component;
  component uartTop is
  generic (
         	-- baudFreq = 16 * baudRate / gcd(clkFreq, 16 * baudRate)
		--   For example if clock frequency is 100MHz and 
		--   baud-rate is 9600, then gcd(16*9600,10^8) is
		--   3200.  So Baud-frequency will be 16*9600/3200 = 48
         	baudFreq  : integer:= 48;
         	-- baudLimit = clkFreq / gcd(clkFreq, 16 * baudRate) - baudFreq
		--   For example if clock frequency is 100MHz and
		--   baud-rate is 9600, the gcd is 3200.  So 
		--   baudLimit = 31202.
         	baudLimit : integer := 31202);
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
  end component;

  -- clock frequency for AJIT platform is 100 MHz.
  constant clock_frequency :integer := 100000000; -- 100 MHz.

  -- baudFreq = 16 * baudRate / gcd(clkFreq, 16 * baudRate)
  --   For example if clock frequency is 100MHz and 
  --   baud-rate is 9600, then gcd(16*9600,10^8) is
  --   6400.  So Baud-frequency will be 16*9600/6400 = 24
  constant uart_baudFreq_9600  : integer := 24;     -- assumes clk frequency = 100MHz.

  -- baudLimit = clkFreq / gcd(clkFreq, 16 * baudRate) - baudFreq
  --   For example if clock frequency is 100MHz and
  --   baud-rate is 9600, the gcd is 6400.  So 
  --   baudLimit = 15601.
  constant uart_baudLimit_9600 : integer := 15601;  -- assumes clk frequency = 100MHz.

  --  16*115200/gcd(100000000,16*115200) = 16*115200/6400 = 115200/400 = 288.
  constant uart_baudFreq_115200  : integer := 288;
  --  100000000/gcd(100000000,16*115200)  - 288 = 100000000/6400 - 288 = 
  constant uart_baudLimit_115200 : integer := 15337; 

end uartPackage;

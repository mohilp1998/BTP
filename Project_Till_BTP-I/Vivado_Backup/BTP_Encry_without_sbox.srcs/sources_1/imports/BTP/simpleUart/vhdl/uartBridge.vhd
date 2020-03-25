library ieee;
use ieee.std_logic_1164.all;
  
entity uartBridge is
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
end entity uartBridge;


architecture Behave of uartBridge is
	type TxFsmState is (IDLE, BUSY);
	signal tx_fsm_state: TxFsmState;
	signal tx_register : std_logic_vector(7 downto 0);

	type RxFsmState is (EMPTY, FULL);
	signal rx_fsm_state: RxFsmState;
	signal rx_register : std_logic_vector(7 downto 0);
begin

	-----------------------------------------------------------------
	-- Tx FSM
	-----------------------------------------------------------------
	process(clk, reset, tx_fsm_state, txBusy, uart_tx_pipe_write_req)
		variable tx_latch_var: std_logic;
		variable next_tx_fsm_state: TxFsmState;
		variable newTxData_var : std_logic;
	begin
		next_tx_fsm_state := tx_fsm_state;
		tx_latch_var := '0';
		newTxData_var := '0';

		case tx_fsm_state is 
			when IDLE => 
				if((txBusy='0') and (uart_tx_pipe_write_req(0) = '1')) then
					next_tx_fsm_state := BUSY;
					tx_latch_var := '1';
				end if;
			when BUSY =>
				newTxData_var := '1';
				if(txBusy = '0') then
					next_tx_fsm_state := IDLE;
				end if;
		end case;

		uart_tx_pipe_write_ack(0) <= tx_latch_var;
		newTxData <= newTxData_var;

		if (clk'event and clk = '1') then
			if(reset = '1') then
				tx_fsm_state <= IDLE;
			else
				tx_fsm_state <= next_tx_fsm_state;
			end if;
				
			if(tx_latch_var = '1') then
				tx_register <= uart_tx_pipe_write_data;
			end if;

		end if;
	end process;
	txData <= tx_register;

	-----------------------------------------------------------------
	-- Rx FSM
	-----------------------------------------------------------------
	process(clk, reset, rx_fsm_state, newRxData, uart_rx_pipe_read_req)
		variable rx_latch_var : std_logic;
		variable next_rx_fsm_state: RxFsmState;
		variable read_ack_var : std_logic;
	begin
		next_rx_fsm_state := rx_fsm_state;
		read_ack_var := '0';
		rx_latch_var := '0';
		case rx_fsm_state is
			when EMPTY =>
				if(newRxData = '1') then
					rx_latch_var := '1';
					next_rx_fsm_state := FULL;
				end if;
			when FULL => 
				read_ack_var := '1';
				if(uart_rx_pipe_read_req(0) = '1') then
					if(newRxData = '1') then
						next_rx_fsm_state := FULL;
						rx_latch_var := '1';
					else
						next_rx_fsm_state := EMPTY;
					end if;
				-- else...
				-- Note: if newRxData becomes '1' and
				--       pipe-read-request has not arrived
				--       then the Rx-data will be lost!
				end if;
		end case;

		uart_rx_pipe_read_ack(0) <= read_ack_var;
		
		if(clk'event and clk='1') then
			if(reset = '1') then
				rx_fsm_state <= EMPTY;
			else
				rx_fsm_state <= next_rx_fsm_state;
			end if;

			if(rx_latch_var = '1') then
				rx_register <= rxData;
			end if;
		end if;
	end process;
	uart_rx_pipe_read_data <= rx_register;

end Behave;


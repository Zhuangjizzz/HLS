module dotprod(
	input	clk,
	input	rst,
	input	start_sig,
	// sram a
	input		[32-1:0]	a_in,
	output	reg	[32-1:0]	a_out,
	output	reg				a_wr_en,
	output	reg				a_rd_en,
	output	reg	[16-1:0]	a_addr,
	// sram b
	input		[32-1:0]	b_in,
	output	reg	[32-1:0]	b_out,
	output	reg				b_wr_en,
	output	reg				b_rd_en,
	output	reg	[16-1:0]	b_addr,
	input		[32-1:0]	n,
	output	reg	[32-1:0]	result,
	output	reg				done_flag
);
// regs
reg	[32-1:0]reg0;
reg	[32-1:0]reg1;
reg	[32-1:0]reg2;
reg	[32-1:0]reg3;
reg	[32-1:0]reg4;
reg	[32-1:0]reg5;
reg	[32-1:0]reg6;
// state registers
reg	[3-1:0]state;
reg	[3-1:0]prev_state;
reg	[3-1:0]branch_state;
// sub state
reg	[3-1:0]sub_state;
// state transition logic
always @(posedge clk or negedge rst) begin
	if (!rst) begin
		state <= 0;
		prev_state <= 0;
		branch_state <= 0;
		sub_state <= 0;
		reg0 <= 0;
		reg1 <= 0;
		reg2 <= 0;
		reg3 <= 0;
		reg4 <= 0;
		reg5 <= 0;
		reg6 <= 0;
		a_addr <= 0;
		a_rd_en <= 0;
		a_wr_en <= 0;
		a_out <= 0;
		b_addr <= 0;
		b_rd_en <= 0;
		b_wr_en <= 0;
		b_out <= 0;
		result <= 0;
		done_flag <= 0;
	end
	else begin
		case(state)
			3'd0: begin
				if (start_sig) begin
					state <= 3'd1;
					prev_state <= 3'd0;
					sub_state <= 3'd0;
					done_flag <= 1'd0;
				end
			end
			3'd1: begin
				if (sub_state == 3'd1) begin
					state <= branch_state;
					prev_state <= state;
					sub_state <= 3'd0;
				end
				else 
					sub_state <= sub_state + 1;
				case (sub_state)
					3'd0: begin
						branch_state <= 3'd2;
						// assign
						reg0 <= 0;
					end
					default: ;
				endcase
			end
			3'd2: begin
				if (sub_state == 3'd3) begin
					state <= branch_state;
					prev_state <= state;
					sub_state <= 3'd0;
				end
				else 
					sub_state <= sub_state + 1;
				case (sub_state)
					3'd0: begin
						// phi
						case (prev_state)
							3'd1: reg3 <= 0;
							3'd3: reg3 <= reg4;
						endcase
						// phi
						case (prev_state)
							3'd1: reg1 <= reg0;
							3'd3: reg1 <= reg2;
						endcase
					end
					3'd1: begin
						// ge
						reg5 <= (reg3 >= n);
					end
					3'd2: begin
						// branch
						branch_state <= reg5 ? 3'd4 : 3'd3;
					end
					default: ;
				endcase
			end
			3'd3: begin
				if (sub_state == 3'd5) begin
					state <= branch_state;
					prev_state <= state;
					sub_state <= 3'd0;
				end
				else 
					sub_state <= sub_state + 1;
				case (sub_state)
					3'd0: begin
						// ld
						a_rd_en <= 1'd1;
						a_addr <= reg3;
						// ld
						b_rd_en <= 1'd1;
						b_addr <= reg3;
						// add
						reg4 <= reg3 + 1;
					end
					3'd1: begin
						a_rd_en <= 1'd0;
						reg5 <= a_in;
						b_rd_en <= 1'd0;
						reg6 <= b_in;
					end
					3'd2: begin
						// mul
						reg5 <= reg5 * reg6;
					end
					3'd3: begin
						// add
						reg2 <= reg1 + reg5;
					end
					3'd4: begin
						// branch
						branch_state <= 3'd2;
					end
					default: ;
				endcase
			end
			3'd4: begin
				if (sub_state == 3'd1) begin
					state <= branch_state;
					prev_state <= state;
					sub_state <= 3'd0;
				end
				else 
					sub_state <= sub_state + 1;
				case (sub_state)
					3'd0: begin
						branch_state <= 3'd5;
						result <= reg1;
						state <= 3'd5;
					end
					default: ;
				endcase
			end
			3'd5: begin
				prev_state <= 3'd5;
				state <= 3'd0;
				done_flag <= 1;
			end
			default: ;
		endcase
	end
end
endmodule

#require "DBCAnalyzer"
require File.dirname(__FILE__) + '/DBCAnalyzer'

target_ECU = "ECU1"

puts "Start Parsing!"
analyzer = CANDBC::CAN_DBC_Analzyer.new

if ARGV.size > 0 and ARGV[0] =~ /.*\.dbc/
    puts "Start Analyze DBC file #{ARGV[0]}"
    can_database = analyzer.AnalyzeDBCFile(ARGV[0])
    puts "Finished DBC file parsing"
    puts "Start Generate Code"
end

if can_database == nil
	exit 0
end

messages_sent_by_me = can_database.messages.select {|msg| msg.transmitter == target_ECU }.sort_by {| msg | msg.id} 
messages_recieved_by_me = can_database.messages.select {|msg| 
	flag = false
	msg.signals.each do |sig|	    
		if sig.receivers.include? target_ECU
			flag = true
		end
	end
	flag
}.sort_by {| msg | msg.id} 

if ARGV.size == 2 and ARGV[1] =~ /.*\.erb/
	template_list = [ARGV[1]]
else
	template_list = Dir["./*.erb"]
end

template_list.each do | temp_file |
	if temp_file =~ /(.*)\.erb/
		f = File.new($1, "w") 
		puts "Generating #{$1}"
		File.open(temp_file) { |fh| 
			erb_engine = ERB.new( fh.read ) 
			f.print erb_engine.result( binding )   
		}
	end
end



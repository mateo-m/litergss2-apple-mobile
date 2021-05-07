require "./lib/LiteRGSS.so"
include LiteRGSS

data = File.binread(File.dirname(File.absolute_path(__FILE__)) + '/image.png')
10.times do
  Bitmap.new(data, true)
end

puts 'Before GC'
GC.start
puts 'After GC'

task :clean_svg do
  Dir["**/*.svg"].each do |svg|
    sh 'svgcleaner', svg, svg
    sh 'xmllint', svg, '-o', svg, '--format'
  end
end

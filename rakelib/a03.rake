namespace :a03 do
  task :e01_docker => :'docker:copy_exercises' do
    cd './docker' do
      sh 'sh', './cluster.sh', 'exec', 'make -C ./a03/e01/'
      sh 'sh', './cluster.sh', 'exec', './a03/e01/heat_stencil_2d_seq'
    end
  end

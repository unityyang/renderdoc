
project_platform=android
project_path=sln_${project_platform}
sln_name=renderdoc_${project_platform}
mkdir -p ${project_path}
cd ${project_path}
cmake -G "Visual Studio 17 2022" -A x64 -DENABLE_GLES=ON -DENABLE_EGL=ON -DENABLE_VULKAN=ON -DANDROID=ON ../renderdoc
mv -n Project.sln ${sln_name}.sln
touch .gitignore
echo "*" >> .gitignore
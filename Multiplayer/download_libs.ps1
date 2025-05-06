# Create directories
New-Item -ItemType Directory -Force -Path "lib\websocketpp"
New-Item -ItemType Directory -Force -Path "lib\boost"
New-Item -ItemType Directory -Force -Path "lib\crow"

# Download WebSocket++
$websocketppUrl = "https://github.com/zaphoyd/websocketpp/archive/refs/tags/0.8.2.zip"
$websocketppZip = "lib\websocketpp.zip"
Invoke-WebRequest -Uri $websocketppUrl -OutFile $websocketppZip
Expand-Archive -Path $websocketppZip -DestinationPath "lib\websocketpp" -Force
Move-Item -Path "lib\websocketpp\websocketpp-0.8.2\websocketpp" -Destination "lib\websocketpp" -Force
Remove-Item -Path "lib\websocketpp\websocketpp-0.8.2" -Recurse -Force
Remove-Item -Path $websocketppZip -Force

# Download Boost
$boostUrl = "https://boostorg.jfrog.io/artifactory/main/release/1.82.0/source/boost_1_82_0.zip"
$boostZip = "lib\boost.zip"
Invoke-WebRequest -Uri $boostUrl -OutFile $boostZip
Expand-Archive -Path $boostZip -DestinationPath "lib\boost" -Force
Move-Item -Path "lib\boost\boost_1_82_0\boost" -Destination "lib\boost" -Force
Remove-Item -Path "lib\boost\boost_1_82_0" -Recurse -Force
Remove-Item -Path $boostZip -Force

# Download Crow
$crowUrl = "https://github.com/CrowCpp/Crow/archive/refs/tags/v1.0+5.zip"
$crowZip = "lib\crow.zip"
Invoke-WebRequest -Uri $crowUrl -OutFile $crowZip
Expand-Archive -Path $crowZip -DestinationPath "lib\crow" -Force
Move-Item -Path "lib\crow\Crow-1.0+5\include" -Destination "lib\crow" -Force
Remove-Item -Path "lib\crow\Crow-1.0+5" -Recurse -Force
Remove-Item -Path $crowZip -Force

Write-Host "Libraries downloaded and extracted successfully!" 
# UE5 Spout2 Plugin

## Overview

This plugin for Unreal Engine 5 allows the engine to receive and send textures using the Spout2 protocol.

**Tested Environment**

* Unreal Engine 5.3
* DirectX 12

**Notes**

* This plugin only supports DirectX 12 and DirectX 11.
* You might need to test it yourself if you're using a different environment.

# Installation

1. Extract the downloaded zip file to the `Plugins` folder of your UE5 project.
2. Regenerate Visual Studio project files.

# Usage

* Add Spout Sender / Receiver to your Blueprint Actor.
  
  <img src="https://github.com/ryanwang0521/UE5-Spout2/blob/master/images/AddComponent.jpg" height="150">

# Spout Receiver

* Use Get First Source / Get List to search available spout sources.
  
  <img src="https://github.com/ryanwang0521/UE5-Spout2/blob/master/images/SearchSource.jpg" height="250">

* Connect to source

  <img src="https://github.com/ryanwang0521/UE5-Spout2/blob/master/images/Connect.jpg" height="250">

* Change receiving speed

  <img src="https://github.com/ryanwang0521/UE5-Spout2/blob/master/images/ChangeFPS.jpg" height="200">

* Stop receiving

  <img src="https://github.com/ryanwang0521/UE5-Spout2/blob/master/images/StopRecv.jpg" height="200">

# Spout Sender

* Start sending

  <img src="https://github.com/ryanwang0521/UE5-Spout2/blob/master/images/StartSend.jpg" height="250">

* Change sending speed

  <img src="https://github.com/ryanwang0521/UE5-Spout2/blob/master/images/ChangeSendFPS.jpg" height="200">

* Stop sending

  <img src="https://github.com/ryanwang0521/UE5-Spout2/blob/master/images/StopSend.jpg" height="150">

  

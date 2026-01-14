# UE5 Spout2 Plugin

A plugin for **Unreal Engine 5** that enables sending and receiving textures using the **Spout2 protocol** for texture sharing between applications.

---

## 🧩 Supported Environment

- **Unreal Engine:** 5.7.1  
- **Graphics API:** DirectX 12 (also supports DirectX 11)

> ⚠️ Other environments may work, but testing is recommended.

---

## 📦 Installation

1. Extract the downloaded `.zip` file into your project's `Plugins` folder.  
2. Regenerate Visual Studio project files.  
3. Rebuild and open your Unreal Engine project.

---

## 🚀 Usage

Add **Spout Sender** or **Spout Receiver** components to your Blueprint Actor.

![Add Component](https://github.com/ryanwang0521/UE5-Spout2/blob/master/images/AddComponent.jpg)

---

### 🎥 Spout Receiver

1. **Search for available sources**  
   Use `Get First Source` or `Get List` to find active Spout sources.  
   ![Search Source](https://github.com/ryanwang0521/UE5-Spout2/blob/master/images/SearchSource.jpg)

2. **Connect to a source**  
   ![Connect](https://github.com/ryanwang0521/UE5-Spout2/blob/master/images/Connect.jpg)

3. **Stop receiving**  
   ![Stop Receiving](https://github.com/ryanwang0521/UE5-Spout2/blob/master/images/StopRecv.jpg)

---

### 📡 Spout Sender

1. **Start sending**  
   ![Start Sending](https://github.com/ryanwang0521/UE5-Spout2/blob/master/images/StartSend.jpg)

2. **Stop sending**  
   ![Stop Sending](https://github.com/ryanwang0521/UE5-Spout2/blob/master/images/StopSend.jpg)

---

## 📝 Notes

- Designed for **DirectX 12**, compatible with **DirectX 11**.  
- Cross-version behavior may vary; please verify in your setup.  
- Useful for real-time texture sharing between UE5 and external visual applications supporting Spout.

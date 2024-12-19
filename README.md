# IR Proximity Sensor-Based Inventory Stock Tracker

Efficient, accurate, and user-friendly inventory tracking designed for small businesses. This project leverages a **PIC16F877A microcontroller** and **IR proximity sensor** to streamline inventory management without the need for complex databases.

## 🎯 Purpose and Benefits

Manual inventory tracking can be time-consuming and error-prone. This system addresses these challenges by offering a **cost-effective, real-time solution** for managing stock levels, tailored for the needs of small businesses.

### Key Benefits:
- **Eliminate Human Error:** Automates stock tracking for greater accuracy.
- **Simple Operation:** Intuitive keypad and LCD interface for easy use.
- **Portable System:** Lightweight, with no reliance on external storage.

## 🔑 Features

- **Real-Time Stock Updates:** Tracks item movement using IR sensor signals.
- **Four Functional Modes:**
  1. **Stock Tracker Mode**: Automatically increment/decrement stock based on sensor detections.
  2. **Browse Item Stocks**: View current stock levels for all tracked items.
  3. **Add Item ID**: Register new items for tracking.
  4. **Remove Item ID**: Delete items from the system.
- **Customizable Tracking**:
  - Supports up to **5 item IDs** during runtime.
  - Assign unique 4-digit IDs to inventory items.
- **Clear Visual Feedback:** Displays current stocks and menu options on a **16x2 LCD**.

---

## 🛠️ Hardware Components

- **PIC16F877A Microcontroller**: Core of the system for signal processing and control.
- **IR Proximity Sensor**: Detects item movements with adjustable distance thresholds.
- **3x4 Matrix Keypad with MM74C922 Encoder**: Facilitates user input for menu navigation.
- **16x2 LCD Display**: Provides real-time feedback on stock and system status.

---

## 📊 Performance and Testing

The system was tested under various conditions to evaluate its reliability. Below are the detection success rates:

| Object Type         | Lit Room | Dark Room |
|---------------------|----------|-----------|
| **White Object**    | 19/20    | 15/20     |
| **Black (Non-Reflective)** | 2/20 | 0/20   |
| **Black (Reflective)**  | 18/20    | 16/20     |

### Observations:
- Reflective objects are detected more reliably.
- Detection improves significantly in well-lit environments.

---

## 🔍 Limitations

1. **Volatile Data**: Stock data resets when the system powers off.
2. **Manual Item Selection**: Users must select the item ID before tracking.
3. **Limited Capacity**: Supports only 5 item IDs at a time, with a max stock of 100 per item.

---

## 💡 Recommendations for Future Enhancements

- Replace the IR sensor with an **ultrasonic sensor** for improved accuracy.
- Add persistent storage to retain data after shutdown.
- Explore options for wireless data transfer and integration with external systems.

---

## 🔄 System Workflow

1. **Startup Menu**: Choose from four operational modes.
2. **Stock Tracking**:
   - Input the item ID to track.
   - Select increment or decrement for stock adjustments.
   - The system updates stock in real-time upon IR sensor detection.
3. **Browsing and Management**:
   - Navigate through item stocks.
   - Add or remove item IDs via the keypad.

---

## 👥 Contributors

- **Matthew Caballero**: Project Manager and Hardware Lead  
- **Pedrito Edzel Resano**: Software Development Lead  
- **Paul Emmanuel Corsino**: Software Co-Developer  

Developed under the **CpE 3201** course at the **University of San Carlos**, Cebu City.

---

## 📅 Project Timeline

| Task                     | Timeline           |
|--------------------------|--------------------|
| Proposal Approval        | April (4th week)  |
| Hardware Setup           | May (1st week)    |
| Software Development     | May (2nd week)    |
| Testing & Debugging      | May (3rd week)    |
| Submission               | May (4th week)    |

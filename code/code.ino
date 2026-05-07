#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <WebServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include "config.h"

// RFID pin definitions
#define RST_PIN 15
#define SS_PIN 2
#define MOSI_PIN 23
#define MISO_PIN 19
#define SCK_PIN 18
#define BUZZER_PIN 5

MFRC522 mfrc522(SS_PIN, RST_PIN);
WebServer server(80);

const char* ssid = "JDE";
const char* password = "JDE123456789";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800, 60000);  // IST offset +5:30

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Adjust to 0x3F if needed

// Allowed Punch-In time threshold (HH:MM:SS, 24hr format)
const char* allowedPunchInTime = "01:30:00";

struct AttendanceRecord {
  String uid;
  String name;
  String punchIn;    // e.g., "Punch-In: 08:55:30"
  String punchOut;   // e.g., "Punch-Out: 18:00:15"
  String date;
  String day;
};
AttendanceRecord attendanceRecords[100];  // Storage for records, index linked to user

// Helper: returns true if punch-in time is after allowed threshold
bool isLate(String punchInTime) {
  int idx = punchInTime.indexOf(": ");
  if (idx == -1) return false;
  String timeStr = punchInTime.substring(idx + 2);  // Extract e.g. "08:55:30"

  int inHour, inMinute, inSecond;
  if (sscanf(timeStr.c_str(), "%d:%d:%d", &inHour, &inMinute, &inSecond) != 3) return false;

  int allowedHour, allowedMinute, allowedSecond;
  if (sscanf(allowedPunchInTime, "%d:%d:%d", &allowedHour, &allowedMinute, &allowedSecond) != 3) return false;

  if (inHour > allowedHour) return true;
  if (inHour == allowedHour && inMinute > allowedMinute) return true;
  if (inHour == allowedHour && inMinute == allowedMinute && inSecond > allowedSecond) return true;

  return false;
}

// Buzzer functions
void beepSuccess() {
  tone(BUZZER_PIN, 1000, 200);
  delay(250);
  noTone(BUZZER_PIN);
}

void beepError() {
  tone(BUZZER_PIN, 500, 300);
  delay(400);
  noTone(BUZZER_PIN);
}

void beepWarning() {
  tone(BUZZER_PIN, 1500, 100);
  delay(150);
  tone(BUZZER_PIN, 800, 100);
  delay(150);
  noTone(BUZZER_PIN);
}

// Get current date and time string for display
String getCurrentDateTime() {
  timeClient.update();
  return getFormattedDate(timeClient.getEpochTime()) + " " + timeClient.getFormattedTime();
}

void setup() {
  Serial.begin(115200);

  // Initialize buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  beepSuccess();  // Startup beep

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("RFID Attendance");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");

  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
  mfrc522.PCD_Init();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected!");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  delay(2000);

  timeClient.begin();
  timeClient.update();

  server.on("/", HTTP_GET, handleRoot);
  server.on("/attendance", HTTP_GET, handleAttendance);
  server.on("/download", HTTP_GET, handleDownload);
  server.begin();
}

void loop() {
  server.handleClient();

  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  String uid = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    uid += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "") + String(mfrc522.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();

  Serial.println("Scanned UID: " + uid);

  bool recognized = false;
  for (int i = 0; i < maxUsers; i++) {
    if (uid == String(uids[i])) {
      timeClient.update();
      String time = timeClient.getFormattedTime();
      String date = getFormattedDate(timeClient.getEpochTime());
      String day = getDayOfWeek(timeClient.getDay());

      if (attendanceRecords[i].uid.isEmpty() || attendanceRecords[i].date != date) {
        // New day or no record for today -> Punch-In
        attendanceRecords[i] = {uid, String(users[i]), "Punch-In: " + time, "", date, day};
        Serial.println(String(users[i]) + " punched in at " + time);

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(users[i]);
        lcd.setCursor(0, 1);
        lcd.print("Punch-In: " + time);
        
        beepSuccess();  // Punch-In success beep
      } else {
        if (attendanceRecords[i].punchOut == "") {
          // Punch-Out missing -> record now
          attendanceRecords[i].punchOut = "Punch-Out: " + time;

          Serial.println(String(users[i]) + " punched out at " + time);

          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(users[i]);
          lcd.setCursor(0, 1);
          lcd.print("Punch-Out: " + time);
          
          beepSuccess();  // Punch-Out success beep
        } else {
          Serial.println(String(users[i]) + " already punched in and out today.");

          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(users[i]);
          lcd.setCursor(0, 1);
          lcd.print("Already punched");
          
          beepWarning();  // Already punched warning beep
        }
      }
      recognized = true;
      break;
    }
  }

  if (!recognized) {
    Serial.println("UID not recognized - Access denied");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("UID Not Found");
    lcd.setCursor(0, 1);
    lcd.print("Access Denied");
    
    beepError();  // Access denied error beep
  }

  mfrc522.PICC_HaltA();
}

void handleRoot() {
  String currentDateTime = getCurrentDateTime();

  String rootHtml =
    "<!DOCTYPE html><html><head>"
    "<meta name='viewport' content='width=device-width, initial-scale=1'>"
    "<title>RFID Attendance System</title>"
    "<style>"
    "body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background: #eef2f7; margin:0; padding:0; }"
    "header { background: #3f51b5; color: white; padding: 20px; text-align: center; }"
    "a.button { display: inline-block; margin: 20px auto; background-color: #4CAF50; color: white;"
    "padding: 15px 30px; font-size: 18px; text-decoration: none; border-radius: 8px;"
    "transition: background-color 0.3s ease; }"
    "a.button:hover { background-color: #45a049; }"
    "main { display: flex; flex-direction: column; align-items: center; margin-top: 50px; }"
    "div.time { text-align:center; font-size: 18px; margin: 10px 0; color: #444; }"
    "</style>"
    "</head><body>"
    "<header><h1>RFID Attendance System</h1></header>"
    "<div class='time'>Current date and time: <strong>" + currentDateTime + "</strong></div>"
    "<main><a href='/attendance' class='button'>View Attendance</a></main>"
    "</body></html>";

  server.send(200, "text/html", rootHtml);
}

void handleAttendance() {
  String currentDateTime = getCurrentDateTime();

  String html =
    "<!DOCTYPE html><html><head>"
    "<meta name='viewport' content='width=device-width, initial-scale=1'>"
    "<title>Attendance Records</title>"
    "<meta http-equiv='refresh' content='10'>"
    "<style>"
    "body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background-color: #f8fafc; margin:0; padding:20px; }"
    "h1 { text-align: center; color: #2c3e50; }"
    "div.time { text-align:center; font-size:18px; margin-bottom:20px; color: #666; }"
    "table { width:90%; max-width: 1000px; margin: 20px auto; border-collapse: collapse; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); }"
    "th, td { padding: 12px 15px; border: 1px solid #ddd; text-align: center; }"
    "th { background-color: #4CAF50; color: white; text-transform: uppercase; }"
    "tr:nth-child(even) { background-color: #f2f9f7; }"
    "tr:hover { background-color: #d1f0e7; }"
    "button { display: block; margin: 30px auto; background-color: #3f51b5; color: white;"
    "border: none; padding: 15px 30px; font-size: 16px; border-radius: 6px; cursor: pointer;"
    "transition: background-color 0.3s ease; }"
    "button:hover { background-color: #303f9f; }"
    "@media screen and (max-width: 600px) {"
    "  table, thead, tbody, th, td, tr { display: block; }"
    "  thead tr { display: none; }"
    "  tr { margin-bottom: 15px; border: 1px solid #ccc; border-radius: 6px; padding: 10px; }"
    "  td { border: none; position: relative; padding-left: 50%; text-align: left; }"
    "  td:before {"
    "    position: absolute; top: 12px; left: 10px; width: 45%; padding-right: 10px;"
    "    white-space: nowrap; font-weight: bold; content: attr(data-label); color: #555;"
    "  }"
    "}"
    "</style>"
    "</head><body>"
    "<h1>Attendance Records</h1>"
    "<div class='time'>Current date and time: <strong>" + currentDateTime + "</strong></div>"
    "<table>"
    "<thead><tr><th>UID</th><th>Name</th><th>Punch-In Time</th><th>Punch-Out Time</th><th>Date</th><th>Day</th><th>Late</th></tr></thead><tbody>";

  for (int i = 0; i < maxUsers; i++) {
    if (!attendanceRecords[i].uid.isEmpty()) {
      String lateStatus = isLate(attendanceRecords[i].punchIn) ? "Yes" : "No";

      html += "<tr>"
              "<td data-label='UID'>" + attendanceRecords[i].uid + "</td>"
              "<td data-label='Name'>" + attendanceRecords[i].name + "</td>"
              "<td data-label='Punch-In Time'>" + attendanceRecords[i].punchIn + "</td>"
              "<td data-label='Punch-Out Time'>" + (attendanceRecords[i].punchOut != "" ? attendanceRecords[i].punchOut : "-") + "</td>"
              "<td data-label='Date'>" + attendanceRecords[i].date + "</td>"
              "<td data-label='Day'>" + attendanceRecords[i].day + "</td>"
              "<td data-label='Late'>" + lateStatus + "</td>"
              "</tr>";
    }
  }

  html += "</tbody></table>"
          "<button onclick=\"window.location.href='/download'\">Download Today's Attendance CSV</button>"
          "</body></html>";

  server.send(200, "text/html", html);
}

void handleDownload() {
  timeClient.update();

  String csv = "UID,Name,Punch-In Time,Punch-Out Time,Date,Day,Late\n";

  for (int i = 0; i < maxUsers; i++) {
    if (!attendanceRecords[i].uid.isEmpty()) {
      String punchInStr = attendanceRecords[i].punchIn;
      punchInStr.replace("Punch-In: ", "");
      String punchOutStr = attendanceRecords[i].punchOut != "" ? attendanceRecords[i].punchOut : "-";
      punchOutStr.replace("Punch-Out: ", "");
      String lateStatus = isLate(attendanceRecords[i].punchIn) ? "Yes" : "No";

      csv += attendanceRecords[i].uid + "," + attendanceRecords[i].name + "," + punchInStr + "," + punchOutStr + "," +
             attendanceRecords[i].date + "," + attendanceRecords[i].day + "," + lateStatus + "\n";
    }
  }

  server.sendHeader("Content-Type", "text/csv");
  server.sendHeader("Content-Disposition",
                    "attachment; filename=attendance_" + getFormattedDate(timeClient.getEpochTime()) +
                    ".csv");
  server.send(200, "text/csv", csv);
}

// Helper: Format epoch timestamp to "YYYY-MM-DD"
String getFormattedDate(unsigned long epoch) {
  time_t rawtime = epoch;
  struct tm* timeinfo = localtime(&rawtime);
  char buffer[11];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d", timeinfo);
  return String(buffer);
}

// Helper: Convert numeric day(0-6) to day name string
String getDayOfWeek(uint8_t day) {
  const char* days[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
  if (day < 7) return days[day];
  return "Unknown";
}
#include <Key.h>
#include <Keypad.h>

#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

const byte rows = 4; //four rows
const byte cols = 3; //three columns

char keys[rows][cols] = {
	{'1','2','3'},
	{'4','5','6'},
	{'7','8','9'},
	{'*','0','#'}
};

byte rowPins[rows] = {10, 11, 12, 13}; //connect to the row pinouts of the keypad
byte colPins[cols] = {7, 8, 9}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, rows, cols );

char *password;
uint8_t password_index = 0; 
uint8_t password_attempts = 1;

uint8_t id;

uint8_t *EEPROM_SIM;

char * get_user_password(uint8_t id);
uint8_t EnrollFinger();
void match_password();
int getFingerprintIDez();



const uint8_t IDLE_FINGERPRINT = 0;
const uint8_t IDLE_PASSWORD = 1;
const uint8_t OPEN_DOOR = 2;
const uint8_t IDLE_ROOT = 3;
const uint8_t PASSWORD_ENTERED = 4;

uint8_t state;

void setup() {
	pinMode(4, OUTPUT);  

    state = IDLE_FINGERPRINT!

    password = (char *) malloc(5 * sizeof(char));
  
    while (!Serial);  // For Yun/Leo/Micro/Zero/...
        delay(500);
  
        Serial.begin(9600);

  
        finger.begin(57600);
  
        if (finger.verifyPassword()) {
            Serial.println("Found fingerprint sensor!");
        } else {
            Serial.println("Did not find fingerprint sensor :(");
            while (1);
        }

	EEPROM_SIM = (uint8_t *) malloc(21 * sizeof(uint8_t));
	EEPROM_SIM[0] = 0;
	Serial.println("Enter a root password ");
	id = 0;
	change_user_password();
	EEPROM_SIM[0] ++;


}

void loop()                     // run over and over again
{
    switch(state){
        case IDLE_FINGERPRINT:
            id = -1;
            id = getFingerprintIDez();
       
            if(id != 255 ) {
                Serial.println("FINGER FOUND!");
                state = IDLE_PASSWORD;
				Serial.println("Enter a password");
            }
            
            break;

           
        case IDLE_PASSWORD:    
            if(password_attempts >= 4){
				Serial.print("Too many password attempts");
				Serial.println(password_attempts);
                password_attempts = 1;
                state = IDLE_FINGERPRINT;
            }
            get_password();
            break;
            
        case OPEN_DOOR:
			digitalWrite(4, HIGH);
			delay(5000);
			digitalWrite(4, LOW);
			state = IDLE_FINGERPRINT;

            break;

        case IDLE_ROOT:
			digitalWrite(4, HIGH);
			delay(100);
			digitalWrite(4, LOW);
			delay(100);
			get_action();
            break;

        case PASSWORD_ENTERED:
            match_password();
              
        default: 
            break;
      }
}

void get_action(){
	char key;
	key = keypad.getKey();

	switch(key){
		case '0':
			id = 0;
			Serial.println("Enter a new password");
			change_user_password();
			state = IDLE_FINGERPRINT;
			break;
		case '1':
			Serial.println("What User? * to enter # to exit");
			char user[3];
			uint8_t index;
			index = 0;
			while(index < 3){
				char user_key;
				user_key = keypad.getKey();
				
				if(user_key != NO_KEY ){
					Serial.print(user_key);
					if(user_key == '*' && index == 0){
						Serial.println("No user entered :(");
						continue;
					}else if(user_key == '*'){
						break;
					}
					if(user_key == '#') return;

					user[index] = user_key;
					index ++;
				}
			}	
			id = atoi(user);
			if(user == 0){
				Serial.println("Invalid User");
				return;
			}
			change_user_password();
			state = IDLE_FINGERPRINT;
			break;
		case '2':
			id = EEPROM_SIM[0];
			while(EnrollFinger() != 169);
			change_user_password();
			EEPROM_SIM[0]++;
			state = IDLE_FINGERPRINT;
			break;
		case NO_KEY:
			break;
		default:
			state = IDLE_FINGERPRINT;
			break;

	}

}	

void get_password(){
    char key;
    key = keypad.getKey();

	if(password_index > 3){
		password[4] == '\0';
        password_attempts++;
        state = PASSWORD_ENTERED;

	}
            
    if (key != NO_KEY){
			  
		password[password_index] = key;
		password_index ++;

		Serial.print("Key ");
		Serial.println(key);
                  
    } 

}

void match_password(){

	boolean root_pass = true;

	for(int i = 0; i < 4; i++){
		if(password[i] != EEPROM_SIM[i+1]){
			root_pass = false;
			break;
		}
	}

	if(root_pass){
		Serial.println("Root Menu. 0 to change root password, 1 to change user password, 2 to enroll new user, any other key to exit");
		password_index = 0;
		state = IDLE_ROOT;
		return;
	}

    char * password_match;
    password_match = get_user_password();
    for(int i = 0; i < 4; i++){
        if(password[i] != password_match[i]){
			state = IDLE_PASSWORD;
			password_index = 0;
			Serial.println("The password was incorrect :( ");
			free(password_match);
			return;
		}

    }
	free(password_match);
    Serial.println("The password was correct :) ");
    state = OPEN_DOOR;

}
void change_user_password(){
	Serial.println("Enter new password");
	uint8_t index = 0;
	uint8_t offset = (id * 4) + 1;
	Serial.print("Offset: ");
	Serial.println(offset);
    
	char key;
    while(index != 4){
		key = keypad.getKey();
		if(key != NO_KEY){
			if(key == '#'){
				continue;
			}
			EEPROM_SIM[index + offset] = key;
			index++;
			Serial.print("Key ");
			Serial.println(key);
		}
	}
	Serial.print("Password: ");
	Serial.print((char) EEPROM_SIM[offset]);
	Serial.print((char) EEPROM_SIM[offset + 1]);
	Serial.print((char) EEPROM_SIM[offset + 2]);
	Serial.println((char) EEPROM_SIM[offset + 3]);

}

char * get_user_password(){
	char * user_password = (char *) malloc(5 * sizeof(char));

	uint8_t offset = (id * 4) + 1;
	Serial.print("Offset: ");
	Serial.println(offset);
    uint8_t index = 0; 
    while(index != 4){
		user_password[index] = EEPROM_SIM[index + offset];
		index++;
	}
	Serial.print(id);
	Serial.print(" 's Password: ");
	Serial.print(user_password[0]);
	Serial.print(user_password[1]);
	Serial.print(user_password[2]);
	Serial.println(user_password[3]);
	

    return user_password;
}



uint8_t EnrollFinger(){

    int p = -1;
    Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
    while (p != FINGERPRINT_OK) {
        p = finger.getImage();
        switch (p) {
            case FINGERPRINT_OK:
                Serial.println("Image taken");
                break;
            
            case FINGERPRINT_NOFINGER:
                Serial.println(".");
                break;

            case FINGERPRINT_PACKETRECIEVEERR:
                Serial.println("Communication error");
                break;

            case FINGERPRINT_IMAGEFAIL:
                Serial.println("Imaging error");
                break;
            
            default:
                Serial.println("Unknown error");
                break;
        }
    }

  // OK success!

    p = finger.image2Tz(1);
    switch (p) {
        case FINGERPRINT_OK:
        Serial.println("Image converted");
        break;

        case FINGERPRINT_IMAGEMESS:
            Serial.println("Image too messy");
            return p;
        case FINGERPRINT_PACKETRECIEVEERR:
            Serial.println("Communication error");
            return p;
        
        case FINGERPRINT_FEATUREFAIL:
            Serial.println("Could not find fingerprint features");
            return p;
        case FINGERPRINT_INVALIDIMAGE:
            Serial.println("Could not find fingerprint features");
            return p;
        default:
            Serial.println("Unknown error");
            return p;
    }
  
    Serial.println("Remove finger");
    delay(2000);
    p = 0;
    while (p != FINGERPRINT_NOFINGER) {
        p = finger.getImage();
    }
    Serial.print("ID "); Serial.println(id);
    p = -1;
    Serial.println("Place same finger again");
    while (p != FINGERPRINT_OK) {
        p = finger.getImage(); 
        switch (p) {
            case FINGERPRINT_OK:
                Serial.println("Image taken");
                break;

            case FINGERPRINT_NOFINGER:
                Serial.print(".");
                break;
            
            case FINGERPRINT_PACKETRECIEVEERR:
                Serial.println("Communication error");
                break;
            case FINGERPRINT_IMAGEFAIL:
                Serial.println("Imaging error");
                break;
            default:
                Serial.println("Unknown error");
                break;
        }
    }


    p = finger.image2Tz(2);
    switch (p) {
        case FINGERPRINT_OK:
            Serial.println("Image converted");
            break;
    
        case FINGERPRINT_IMAGEMESS:
            Serial.println("Image too messy");
            return p;
        
        case FINGERPRINT_PACKETRECIEVEERR:
            Serial.println("Communication error");
            return p;
        
        case FINGERPRINT_FEATUREFAIL:
            Serial.println("Could not find fingerprint features");
            return p;
        
        case FINGERPRINT_INVALIDIMAGE:
            Serial.println("Could not find fingerprint features");
            return p;
        default:
            return p;
    }
  
    Serial.print("Creating model for #");  Serial.println(id);
  
    p = finger.createModel();
    if (p == FINGERPRINT_OK) {
        Serial.println("Prints matched!");
    } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
        Serial.println("Communication error");
        return p;
    } else if (p == FINGERPRINT_ENROLLMISMATCH) {
        Serial.println("Fingerprints did not match");
        return p;
    } else {
        Serial.println("Unknown error");
        return p;
    }   
  
    Serial.print("ID "); Serial.println(id);
    p = finger.storeModel(id);
    if (p == FINGERPRINT_OK) {
        Serial.println("Stored!");
    } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
        Serial.println("Communication error");
        return p;
    } else if (p == FINGERPRINT_BADLOCATION) {
        Serial.println("Could not store in that location");
        return p;
    } else if (p == FINGERPRINT_FLASHERR) {
        Serial.println("Error writing to flash");
        return p;
    } else {
        Serial.println("Unknown error");
        return p;
    }   
	return 169;
}

int getFingerprintIDez() {
    uint8_t p = finger.getImage();
    if (p != FINGERPRINT_OK)  return -1;

	p = finger.image2Tz();
	if (p != FINGERPRINT_OK)  return -1;

	p = finger.fingerFastSearch();
    if (p != FINGERPRINT_OK)  return -1;
  
	// found a match!
	Serial.print("Found ID #"); Serial.print(finger.fingerID); 
	Serial.print(" with confidence of "); Serial.println(finger.confidence);
	return finger.fingerID; 
}

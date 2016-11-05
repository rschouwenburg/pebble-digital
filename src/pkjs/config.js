module.exports =

[
  {
    "type": "heading",
    "id": "main-heading",
    "defaultValue": "DIGITAL CONFIGURATION",
    "size": 1
  },
  {
  "type": "section",
  "capabilities": ["BW"],
  "items": [
    {
      "type": "heading",
      "defaultValue": "Theme",
    },
  
    {
      "type": "color",
      "messageKey": "THEME",
      "defaultValue": "000000",
      "label": "Background Color",
      "sunlight": false,
      "layout": "BLACK_WHITE"
    } ]
  },
  
  
  {
  "type": "section",
  "items": [
    {
      "type": "heading",
      "defaultValue": "Phone bluetooth connection"
    },
    {
      "type": "toggle",
      "messageKey": "VIBRATE",
      "label": "Vibrate",
      "defaultValue": true,
      "description": "Turn on or off the vibration on your Pebble when there is a change in the bluetooth connection."
    }
  ]
  },
  {
  "type": "section",
  "items": [
    {
      "type": "heading",
      "defaultValue": "Settings"
    },
    {
      "type": "select",
      "messageKey": "CELSIUS",
      "defaultValue": "0",
      "label": "Temperature in:",
      "options": [
        { 
          "label": "Fahrenheit", 
          "value": "0"
        },
        { 
          "label": "Celsius", 
          "value": "1"
        },
      ]
    },
    {
      "type": "select",
      "messageKey": "EUDATE",
      "defaultValue": "0",
      "label": "Show date as:",
      "options": [
        { 
          "label": "FRI MARCH 29", 
          "value": "0"
        },
        { 
          "label": "FRI 29 MARCH", 
          "value": "1"
        },
      ]
    }
  ]
  },
  {
  "type": "section",
  "items": [
    {
      "type": "heading",
      "defaultValue": "Features"
    },
    {
      "type": "toggle",
      "messageKey": "TOPBAR",
      "label": "Show top bar",
      "defaultValue": true,
    },
    {
      "type": "toggle",
      "messageKey": "BOTTOMBAR",
      "label": "Show bottom bar",
      "defaultValue": true,
    }
  ]
  },
  {
    "type": "submit",
    "defaultValue": "Save"
  }
];
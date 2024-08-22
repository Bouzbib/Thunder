using System;
using System.Collections;
using System.IO.Ports;
using System.Threading;
using UnityEngine;
using System.Globalization;
using System.Collections.Generic;


public class ArduinoManager : MonoBehaviour {
	public bool reset;
    public string arduinoCOM;

    [HideInInspector]
    public bool looping = true;

    [HideInInspector]
    public bool boardConnected;

    private SerialPort stream;
    private Thread thread;

	private string[] dataSplit;

    public bool IsLooping() {
        lock (this) {
            return this.looping;
        }
    }

    private void Start() {
        //Run communication thread
        this.thread = new Thread(this.threadLoop);
        this.thread.Start();

    }

    void Update()
    {
    	if(reset)
    	{
    		// StartCoroutine(ResetArduino());
    		ResetArduino();
    		reset = false;
    	}

    }

    void ResetArduino()
    {
		// this.stopThread();
  //   	this.thread = new Thread(this.threadLoop);
  //       this.thread.Start();
		// yield return new WaitForSeconds(1.0f);
    	this.thread = new Thread(this.threadLoop);
        this.thread.Start();

    }


    public void threadLoop() {
        this.connectToArduino();

        // Looping
        while (this.IsLooping()) {
           
        	// this.writeToArduino("a");
            // Read from Arduino
            string result = this.readFromArduino();

            if (result != null) {
            	try{
                    dataSplit = result.Split(' ');
                    try{
                    	// this.debugState = (dataSplit[0]);

                    	// this.debugE0 = int.Parse(dataSplit[1], CultureInfo.InvariantCulture);
                    	// this.debugE1 = int.Parse(dataSplit[2], CultureInfo.InvariantCulture);
                    	// this.debugE2 = int.Parse(dataSplit[3], CultureInfo.InvariantCulture);

                    	// this.debugD0 = int.Parse(dataSplit[4], CultureInfo.InvariantCulture);
                    	// this.debugD1 = int.Parse(dataSplit[5], CultureInfo.InvariantCulture);

                    	// this.debugD2 = int.Parse(dataSplit[6], CultureInfo.InvariantCulture);
                    	// // this.debugD11 = int.Parse(dataSplit[7], CultureInfo.InvariantCulture);

                    	// // this.debugD20 = int.Parse(dataSplit[8], CultureInfo.InvariantCulture);
                    	// // this.debugD21 = int.Parse(dataSplit[9], CultureInfo.InvariantCulture);
                    	
                    	// this.debugDuration = int.Parse(dataSplit[7], CultureInfo.InvariantCulture);
                     //    this.debugAsynchrony = int.Parse(dataSplit[8], CultureInfo.InvariantCulture);
                     //    this.debugTotalDischarge = int.Parse(dataSplit[9], CultureInfo.InvariantCulture);
                        // this.debugDelayRest = int.Parse(dataSplit[13], CultureInfo.InvariantCulture);

                    }
                    catch (Exception e)
                    {
                        Debug.LogError("ReadArduino: " + e.Message);
                        // throw;
                    }

                }
                catch (Exception e)
                {
                    Debug.LogError("DataSplit error: " + e.Message);
                    // this.debugState = "";
                    // this.debugElectrodeNumber = 0;
                    // this.debugDuration = 0;
                    // throw;
                }
                               
                this.stream.DiscardInBuffer();
            }
        }

        Debug.Log("stream closing");
        this.stream.Close();
    }

    public void connectToArduino() {
        if (int.Parse(this.arduinoCOM) >= 0) 
        {
            this.stream = new SerialPort("COM" + this.arduinoCOM, 115200);
            this.stream.ReadBufferSize = 8192;
            this.stream.WriteBufferSize = 128;
            this.stream.ReadTimeout = 500;

            this.stream.Parity = Parity.None;
            this.stream.StopBits = StopBits.One;

            try {
                this.stream.Open();
                Debug.Log("Communication port with Arduino is ready.");
                boardConnected = true;
            } catch (Exception e) {
                Debug.LogError(e.Message);
                boardConnected = false;
                throw;
            }

        } else {
            Debug.Log("Unable to open communication port with Arduino.");
        }
    }

    public string readFromArduino(int timeout = 50) {
        try {
            return this.stream.ReadLine();
        } catch (TimeoutException e) {
            // Debug.Log("TimeoutError in read: " + e.Message);

            // throw;
            return null;
        } catch (Exception e) {
            Debug.Log("Read error: " + e.Message);
            // throw;
            return null;
        }
    }

    public void writeToArduino(string message) {
        if (this.stream != null) {
            this.stream.WriteLine(message);
            this.stream.BaseStream.Flush();
        }
    }

    public void stopThread() {
        lock (this) {
            this.looping = false;
        }
    }

    private void OnApplicationQuit() {
        this.stopThread();
    }

}



// %LOCALAPPDATA%\Arduino15\packages\arduino\tools\avrdude\6.3.0-arduino17/bin/avrdude -C%LOCALAPPDATA%\Arduino15\packages\arduino\tools\avrdude\6.3.0-arduino17/etc/avrdude.conf -v -patmega328p -carduino -PCOM18 -b115200 -D -Uflash:w:ActivateElectrodeFromUnity-Timeouts.ino.hex:i
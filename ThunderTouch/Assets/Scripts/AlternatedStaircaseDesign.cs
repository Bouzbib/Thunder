using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;
using UnityEngine.UI;
using System.IO;

public class AlternatedStaircaseDesign : MonoBehaviour
{
	[HideInInspector]
	public ArduinoManager arduinoCmd;
	public string userName;

	private float duration = 0.25f;
	public int frequency, intensity;
	[Serializable]
	public enum direction
	{
		ASCENDING,
		DESCENDING
	}

	[Serializable]
	public enum positionStimuli
	{
		FINGER,
		PALM
	}

	public positionStimuli positionXP;

	[Serializable]
	public struct serieStimulus
	{
		public int currentDelay;
		public int previousDelay;
		public int incrementDelay;
		public string currentDirection;
		public int reversal;
	}

	public int nbMaxReversals = 4;
	public serieStimulus serieA, serieB;
	private int oldReversalA, oldReversalB;
	public int startDelaySerieA, startDelaySerieB;

	private int state;
	private string time0;
	[HideInInspector]
	public Material materialChoosing, materialNoChoice, materialValidation;

	private bool finishedTesting, firstChoice;
	private int oneStimulus;
	[HideInInspector]
	public TextMeshPro consignes;

	private string path;
	private StreamWriter writer;

	private int oldTrial, nbTrials;
	private bool showButtons;
	private bool start;

	public List<(int, int, string, int, int)> resultsAToWrite, resultsBToWrite;
	private int count;


    // Start is called before the first frame update
    void Start()
    {
    	arduinoCmd = this.GetComponent<ArduinoManager>();

	    serieA.currentDelay = startDelaySerieA;
        serieA.previousDelay = startDelaySerieA;
        serieA.currentDirection = direction.DESCENDING.ToString();

        serieB.currentDelay = startDelaySerieB;
        serieB.previousDelay = startDelaySerieB;
        serieB.currentDirection = direction.ASCENDING.ToString();

        time0 = System.DateTime.Now.ToString("ddMMyyyy-HHmm");
		consignes.text = "Press Enter to start the stimuli.";
		state = -1;

    	firstChoice = false;
    	serieA.incrementDelay = 5;
    	serieB.incrementDelay = 5;

    	resultsAToWrite = new List<(int, int, string, int, int)> {};
		resultsBToWrite = new List<(int, int, string, int, int)> {};


    	WriteResults(resultsAToWrite, "resultsA");
		WriteResults(resultsBToWrite, "resultsB");

    }

    // Update is called once per frame
    void Update()
    {
        if(showButtons)
    	{
	    	GameObject.Find("ButtonLeft").GetComponent<MeshRenderer>().enabled = true;
			GameObject.Find("ButtonRight").GetComponent<MeshRenderer>().enabled = true;
    		GameObject.Find("ButtonLeft").GetComponentInChildren<TextMeshPro>().text = "One";
	    	GameObject.Find("ButtonRight").GetComponentInChildren<TextMeshPro>().text = "Two \n or More";
    	}
    	else
    	{
    		GameObject.Find("ButtonLeft").GetComponent<MeshRenderer>().enabled = false;
			GameObject.Find("ButtonRight").GetComponent<MeshRenderer>().enabled = false;
	    	GameObject.Find("ButtonLeft").GetComponentInChildren<TextMeshPro>().text = "";
	    	GameObject.Find("ButtonRight").GetComponentInChildren<TextMeshPro>().text = "";
    	}

    	switch(state)
    	{
    		case -1:
    			consignes.text = "In this experiment you'll perceive different types of stimuli. \nYou'll have to tell us whether you felt one stimulus or more stimuli, \ne.g. two or more. \n Press Enter to continue.";
    			if(Input.GetKeyDown(KeyCode.KeypadEnter))
				{
					state = 0;
				}
    			break;

    		case 0:
    			serieA.previousDelay = serieA.currentDelay;
    			serieB.previousDelay = serieB.currentDelay;

				oldReversalA = serieA.reversal;
				oldReversalB = serieB.reversal;

				oldTrial = nbTrials;
    			firstChoice = false;

    			showButtons = false;
    			oneStimulus = -1;
				consignes.text = "Press Enter to start the stimuli.";

				if(Input.GetKeyDown(KeyCode.KeypadEnter))
				{
					start = true;
				}

				if(start)
				{
					if((nbTrials % 2 == 0))
					{
						if(serieA.reversal < nbMaxReversals)
						{
							StartCoroutine(LaunchStimulus("serieA"));
						// HERE START WITH DELAY A ASCENDING
						}
						else
						{
							if(serieB.reversal < nbMaxReversals)
							{
								StartCoroutine(LaunchStimulus("serieB"));
								// HERE START SERIE B AGAIN
							}
						}
					}
					else
					{
						if(serieB.reversal < nbMaxReversals)
						{
							StartCoroutine(LaunchStimulus("serieB"));
						// HERE START WITH DELAY B
						}
						else
						{
							if(serieA.reversal < nbMaxReversals)
							{
								StartCoroutine(LaunchStimulus("serieA"));
								// HERE START SERIE A AGAIN
							}
						}
					}
				}

				if(finishedTesting || Input.GetKeyDown(KeyCode.Space))
				{
					state = 1;
				}

				break;

			case 1:
				finishedTesting = false;
				start = false;

				showButtons = true;
				if(Input.GetKeyDown(KeyCode.RightArrow) || (Input.GetKeyDown(KeyCode.LeftArrow)))
				{
					firstChoice = true;
				}
				if(firstChoice)
				{
					if(Input.GetKeyDown(KeyCode.RightArrow))
					{
						GameObject.Find("ButtonRight").GetComponent<MeshRenderer>().material = materialChoosing;
						GameObject.Find("ButtonLeft").GetComponent<MeshRenderer>().material = materialNoChoice;	
						oneStimulus = 1;
					}

					if(Input.GetKeyDown(KeyCode.LeftArrow))
					{
						GameObject.Find("ButtonRight").GetComponent<MeshRenderer>().material = materialNoChoice;
						GameObject.Find("ButtonLeft").GetComponent<MeshRenderer>().material = materialChoosing;	
						oneStimulus = 0;			
					}

					if(Input.GetKeyDown(KeyCode.KeypadEnter))
					{
						if(oneStimulus == 0)
						{
							GameObject.Find("ButtonRight").GetComponent<MeshRenderer>().material = materialNoChoice;
							GameObject.Find("ButtonLeft").GetComponent<MeshRenderer>().material = materialValidation;	

						}
						else
						{
							GameObject.Find("ButtonRight").GetComponent<MeshRenderer>().material = materialValidation;
							GameObject.Find("ButtonLeft").GetComponent<MeshRenderer>().material = materialNoChoice;
						}
						state = 2;
					}
				}
				else
				{
					GameObject.Find("ButtonRight").GetComponent<MeshRenderer>().material = materialNoChoice;
					GameObject.Find("ButtonLeft").GetComponent<MeshRenderer>().material = materialNoChoice;	
					oneStimulus = -1;		
				}
				consignes.text = "I felt ONE or MORE stimulus. \n Select your answer with the L/R arrows. Press Enter to confirm.";

				if(Input.GetKeyDown(KeyCode.Keypad0))
				{
					state = 0;
				}
				if(Input.GetKeyDown(KeyCode.Space))
				{
					state = 2;
				}
				break;

			case 2:
				consignes.text = "Press 0 to go back. Press Enter to confirm your answer.";
				if(Input.GetKeyDown(KeyCode.Keypad0))
				{
					state = 1;
				}

				if(Input.GetKeyDown(KeyCode.KeypadEnter))
				{
					state = 3;
				}
				break;

			case 3:
				showButtons = false;
				// WRITE RESULTS AS: nbTrials, currentSerie, currentSerieDelay, result, nbReversal

				if((nbTrials % 2 == 0))
				{
					if(serieA.reversal < nbMaxReversals)
					{
						resultsAToWrite.Add((nbTrials, serieA.currentDelay, serieA.currentDirection, serieA.reversal, oneStimulus));
						if(serieA.currentDirection == direction.DESCENDING.ToString())
						{
							if(oneStimulus == 0)
							{
								serieA.reversal = oldReversalA + 1;
								Debug.Log("Reversing A");
								serieA.currentDelay = serieA.previousDelay + serieA.incrementDelay;
								serieA.currentDirection = direction.ASCENDING.ToString();
							}
							else
							{
								serieA.currentDelay = serieA.previousDelay - serieA.incrementDelay;								
							}
						}
						else
						{
							if(oneStimulus == 1)
							{
								serieA.reversal = oldReversalA + 1;
								Debug.Log("Reversing A");
								serieA.currentDelay = serieA.previousDelay - serieA.incrementDelay;
								serieA.currentDirection = direction.DESCENDING.ToString();
							}
							else
							{
								serieA.currentDelay = serieA.previousDelay + serieA.incrementDelay;
							}
						}

						WriteResults(resultsAToWrite, "resultsA");

						
						// consignes.text = "Press Enter to start the stimuli.";
						// Debug.Log("Now Serie B.");


						// Debug.Log("NumTrials " + nbTrials);
						nbTrials = oldTrial + 1;
						// state = 0;
					}
					else
					{
						if(serieB.reversal < nbMaxReversals)
						{
							resultsBToWrite.Add((nbTrials, serieB.currentDelay, serieB.currentDirection, serieB.reversal, oneStimulus));
							if(serieB.currentDirection == direction.DESCENDING.ToString())
							{
								if(oneStimulus == 0)
								{
									serieB.reversal = oldReversalB + 1;
									serieB.currentDelay = serieB.previousDelay + serieB.incrementDelay;
									Debug.Log("Reversing B");
									serieB.currentDirection = direction.ASCENDING.ToString();
								}
								else
								{
									serieB.currentDelay = serieB.previousDelay - serieB.incrementDelay;
								}
							}
							else
							{
								if(oneStimulus == 1)
								{
									serieB.reversal = oldReversalB + 1;
									serieB.currentDelay = serieB.previousDelay - serieB.incrementDelay;
									Debug.Log("Reversing B");
									serieB.currentDirection = direction.DESCENDING.ToString();
								}
								else
								{
									serieB.currentDelay = serieB.previousDelay + serieB.incrementDelay;

								}
							}
							// consignes.text = "Press Enter to start the stimuli.";
							Debug.Log("Now Serie A.");


							Debug.Log("NumTrials " + nbTrials);
							WriteResults(resultsBToWrite, "resultsB");

							nbTrials = oldTrial + 1;
							// state = 0;
						}
						// else
						// {
						// 	consignes.text = "Experiment is over. Thank you!";
						// 	// WRITE
						// 	Debug.Break();
						// }
					}

				}
				else
				{
					if(serieB.reversal < nbMaxReversals)
					{
						resultsBToWrite.Add((nbTrials, serieB.currentDelay, serieB.currentDirection, serieB.reversal, oneStimulus));
						if(serieB.currentDirection == direction.DESCENDING.ToString())
						{
							if(oneStimulus == 0)
							{
								serieB.reversal = oldReversalB + 1;
								serieB.currentDelay = serieB.previousDelay + serieB.incrementDelay;
								Debug.Log("Reversing B.");
								serieB.currentDirection = direction.ASCENDING.ToString();
							}
							else
							{
								serieB.currentDelay = serieB.previousDelay - serieB.incrementDelay;
							}
						}
						else
						{
							if(oneStimulus == 1)
							{
								serieB.reversal = oldReversalB + 1;
								Debug.Log("Reversing B.");
								serieB.currentDelay = serieB.previousDelay - serieB.incrementDelay;
								serieB.currentDirection = direction.DESCENDING.ToString();
							}
							else
							{
								serieB.currentDelay = serieB.previousDelay + serieB.incrementDelay;
							}
						}
						Debug.Log("Now Serie A.");
						// consignes.text = "Press Enter to start the stimuli.";

						Debug.Log("NumTrials " + nbTrials);
						WriteResults(resultsBToWrite, "resultsB");

						nbTrials = oldTrial + 1;
						// state = 0;
					}
					else
					{
						if(serieA.reversal < nbMaxReversals)
						{
							resultsAToWrite.Add((nbTrials, serieA.currentDelay, serieA.currentDirection, serieA.reversal, oneStimulus));
							if(serieA.currentDirection == direction.DESCENDING.ToString())
							{
								if(oneStimulus == 0)
								{
									serieA.reversal = oldReversalA + 1;
									Debug.Log("Reversing A.");
									serieA.currentDelay = serieA.previousDelay + serieA.incrementDelay;
									serieA.currentDirection = direction.ASCENDING.ToString();
								}
								else
								{
									serieA.currentDelay = serieA.previousDelay - serieA.incrementDelay;
								}
							}
							else
							{
								if(oneStimulus == 1)
								{
									serieA.reversal = oldReversalA + 1;
									Debug.Log("Reversing A.");
									serieA.currentDelay = serieA.previousDelay - serieA.incrementDelay;
									serieA.currentDirection = direction.DESCENDING.ToString();
								}
								else
								{
									serieA.currentDelay = serieA.previousDelay + serieA.incrementDelay;
								}
							}
							// consignes.text = "Press Enter to start the stimuli.";
							Debug.Log("Now Serie B.");
							Debug.Log("NumTrials " + nbTrials);
							WriteResults(resultsAToWrite, "resultsA");
							nbTrials = oldTrial + 1;
						}
					}

				}


				if((serieA.reversal == nbMaxReversals) && (serieB.reversal == nbMaxReversals))
				{
					consignes.text = "Experiment is over. Thank you!";
					// WriteResults(resultsAToWrite, "resultsA");
					// WriteResults(resultsBToWrite, "resultsB");

					Debug.Break();
				}
				else
				{
					consignes.text = "Press Enter to start the stimuli.";
					state = 0;
				}

				break;
    	}

    }

    IEnumerator LaunchStimulus(string serieID)
    {
    	int delayToSend;
    	start = false;
    	if(serieID == "serieA")
    	{
    		delayToSend = serieA.currentDelay;
    		Debug.Log("Trial: " + nbTrials + "; Serie A. Delay: " + delayToSend + "; NbReversal: " + serieA.reversal);
    	}
    	else
    	{
    		delayToSend = serieB.currentDelay;
    		Debug.Log("Trial: " + nbTrials + "; Serie B. Delay: " + delayToSend + "; NbReversal: " + serieB.reversal);
    	}
    	// Serial communication send delayToSend;
		string toArduino = "11 1 " + intensity + " " + frequency + " " + (int)(frequency*duration) + " " + delayToSend;
    	arduinoCmd.writeToArduino(toArduino);
		Debug.Log(toArduino);
    	yield return new WaitForSeconds(1.0f);
    	finishedTesting = true;

    }

    void WriteResults(List<(int, int, string, int, int)> results, string serieID)
    {
    	if(serieID == "resultsA")
    	{
	    	path = "Assets/Resources/DataCollection/TemporalStaircase-" + userName + "-Freq" + frequency + "-Position" + positionXP.ToString() + "-" + time0 + "-SerieA.csv";
			count = resultsAToWrite.Count - 1;
    	}
    	else
    	{
	    	path = "Assets/Resources/DataCollection/TemporalStaircase-" + userName + "-Freq" + frequency + "-Position" + positionXP.ToString() + "-" + time0 + "-SerieB.csv";
			count = resultsBToWrite.Count - 1;
    	}

    	if(state == -1)
    	{
    		writer = new StreamWriter(path, true);
			writer.WriteLine("TrialID;CurrentDelay;CurrentDirection;NbReversal;OneOrMoreStimulus");
			writer.Close();
    	}
    	else
    	{
    		writer = new StreamWriter(path, true);
			writer.WriteLine(results[count].Item1 + ";" + results[count].Item2 + ";" + results[count].Item3 + ";" + results[count].Item4 + ";" + results[count].Item5);

    		// for(int i = 0; i < results.Count; i++)
			// {
			// 	writer.WriteLine(results[i].Item1 + ";" + results[i].Item2 + ";" + results[i].Item3 + ";" + results[i].Item4 + ";" + results[i].Item5);
			// }
			writer.Close();
    	}
    }
}

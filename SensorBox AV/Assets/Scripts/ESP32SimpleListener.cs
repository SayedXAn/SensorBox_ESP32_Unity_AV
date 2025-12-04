using UnityEngine;
using UnityEngine.Video;
using System.Net.Sockets;
using System.IO;
using System.Collections;
using Unity.VisualScripting;

public class ESP32SimpleListener : MonoBehaviour
{
    public string espIP = "192.168.4.1";
    public int espPort = 3333;

    public VideoPlayer videoPlayer;

    private TcpClient client;
    private StreamReader reader;
    public RenderTexture rt;
    private string videoUrl;

    void Start()
    {
        videoUrl = Application.dataPath + "/Content.mp4";
        StartCoroutine(ConnectAndListen());
    }
    private void Update()
    {
        if(Input.GetKeyUp(KeyCode.Escape))
        {
            Application.Quit();
        }

        if (Input.GetKeyUp(KeyCode.R))
        {
            ResetVideo();
        }
    }
    IEnumerator ConnectAndListen()
    {
        // Try to connect
        while (client == null || !client.Connected)
        {
            Debug.Log("Connecting to ESP32...");
            client = new TcpClient();

            var result = client.BeginConnect(espIP, espPort, null, null);
            yield return new WaitForSeconds(1f);

            if (!result.IsCompleted)
                continue;

            client.EndConnect(result);
            reader = new StreamReader(client.GetStream());
            Debug.Log("Connected!");
        }

        // Listen continuously
        while (true)
        {
            if (client.Available > 0)
            {
                string msg = reader.ReadLine();
                Debug.Log("Received: " + msg);

                if (msg == "PLAY")
                {
                    PlayVideo();
                }
            }

            yield return null; // non-blocking
        }
    }

    void PlayVideo()
    {
        if (videoPlayer == null) return;
        rt.Release();
        videoPlayer.Stop();
        videoPlayer.url = videoUrl;
        videoPlayer.Play();
    }

    // Manual reset button
    public void ResetVideo()
    {
        videoPlayer.Stop();
        rt.Release();
    }

    void OnApplicationQuit()
    {
        client?.Close();
    }
}

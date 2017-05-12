//
//  ViewController.swift
//  RESTTest
//
//  Created by Lewis Collins on 05/05/2017.
//  Copyright © 2017 Plantronics. All rights reserved.
//

import Cocoa

class ViewController: NSViewController, NSWindowDelegate {
    @IBOutlet var logText: NSTextView!
    
    override func viewDidLoad() {
        super.viewDidLoad()

        // Do any additional setup after loading the view.
    }

    override var representedObject: Any? {
        didSet {
        // Update the view, if already loaded.
        }
    }
    
    override func viewDidAppear() {
        self.view.window?.delegate = self
    }
    
    func windowShouldClose(_ sender: Any) {
        NSApplication.shared().terminate(self)
    }

    @IBAction func ButtonInvoked(_ sender: Any) {
        SendRESTRequest(url: "http://127.0.0.1:32017/Spokes/DeviceServices/Info", callbackid: CallbackId.deviceInfo,
                        logtext: "Querying device info...")
        
    }
    
    func RESTCallback(result: String, callbackid: CallbackId) {
        DispatchQueue.main.async() {
            switch callbackid {
            case .deviceInfo:
                self.logText.insertText(result, replacementRange: self.logText.selectedRange())
            }
        }
    }

    func SendRESTRequest(url: String, callbackid: CallbackId, logtext: String = "Sending REST Request...") -> Void {
        let logTextStr = String(logtext + "\n")
        self.logText.insertText(logTextStr!, replacementRange: self.logText.selectedRange())
        let session = URLSession.shared
        let url = URL(string: "http://127.0.0.1:32017/Spokes/DeviceServices/Info")!
        let task = session.dataTask(with: url) { (data, _, _) -> Void in
            if let data = data {
                let string = String(data: data, encoding: String.Encoding.utf8)
                self.RESTCallback(result: string!, callbackid: callbackid)
            }
        }
        task.resume()
    }
    
    enum CallbackId {
        case deviceInfo
    }
}


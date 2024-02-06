

export class Client{
    constructor(){
        this.setParametersCallback = null;
        this.ws = new WebSocket('ws://' + window.location.host + "/ui");

        this.ws.onopen = event=>{
            console.log('Connection opened:', event);
        };

        this.ws.addEventListener("message", (event)=>{
            const message = JSON.parse(event.data);
            // console.log('Message:', message);
            this.onMessage(message);
        })

        this.ws.onerror = (error) => {
            console.error('WebSocket Error:', error);
        };

        this.ws.onclose = (event) => {
            if (event.wasClean) {
                console.log('Connection closed cleanly, code=', event.code, 'reason=', event.reason);
            } else {
                console.error('Connection died');
            }
        };
    }
    setEnvironmentCallbacks({setParametersCallback, setStateCallback, setActionCallback}){
        this.setParametersCallback = setParametersCallback;
        this.setStateCallback = setStateCallback;
        this.setActionCallback = setActionCallback;
    }


    onMessage(message){
        let {channel, data} = message
        if(channel === "setParameters"){
            if(this.setParametersCallback){
                this.setParametersCallback(data)
            }
        }
        else{
            if(channel === "setState"){
                if(this.setStateCallback){
                    this.setStateCallback(data)
                }
            }
            else{
                if(channel === "setAction"){
                    if(this.setActionCallback){
                        this.setActionCallback(data)
                    }
                }
            }
        }
    }
    sendMessage(channel, data){
        this.ws.send(JSON.stringify({channel, data}));
    }
}

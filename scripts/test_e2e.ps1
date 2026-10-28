# E2E test: send binary frame (magic 0xCCDD + FT_SENSOR + GPS JSON) via WebSocket
$ws = [System.Net.WebSockets.ClientWebSocket]::new()
try {
    $ws.ConnectAsync([Uri]"ws://127.0.0.1:9000/ws", [Threading.CancellationToken]::None).Wait()
    Write-Host "WebSocket connected"

    $json = '{"type":"GPSLocation","data":{"ts":1760000000,"lat":39.9,"lng":116.4,"alt":100.5,"fix":1,"sat":12}}'
    $payload = [Text.Encoding]::UTF8.GetBytes($json)
    $frame = New-Object byte[] (7 + $payload.Length)
    $frame[0] = 0xCC; $frame[1] = 0xDD          # magic
    $frame[2] = 0x02                             # FT_SENSOR
    [BitConverter]::GetBytes([uint32]$payload.Length).CopyTo($frame, 3)  # length (LE)
    $payload.CopyTo($frame, 7)

    $ws.SendAsync([ArraySegment[byte]]::new($frame), [Net.WebSockets.WebSocketMessageType]::Binary, $true, [Threading.CancellationToken]::None).Wait()
    Write-Host "Binary frame sent: $($frame.Length) bytes"
    Start-Sleep -Seconds 2
} finally {
    $ws.Dispose()
}

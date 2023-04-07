import { Edit } from "@mui/icons-material";
import {
  Button,
  Container,
  IconButton,
  Paper,
  Slider,
  Stack,
  Table,
  TableBody,
  TableCell,
  TableContainer,
  TableHead,
  TableRow,
  TextField,
  Typography,
} from "@mui/material";
import axios from "axios";
import { h, Fragment } from "preact";
import { useEffect, useState } from "preact/hooks";
import Telemetry from "src/models/telemetry";
import ReactApexChart from "react-apexcharts";

import Station from "../models/station";

interface Props {
  stationId: number;
}

// Note: `user` comes from the URL, courtesy of our router
const StationPage = ({ stationId }: Props) => {
  const [brightness, setBrightness] = useState<number>(0);
  const [station, setStation] = useState<Station>();
  const [telemetry, setTelemetry] = useState<Telemetry[]>([]);
  const [editing, setEditing] = useState(false);
  const [newName, setNewName] = useState("");
  const [minutes, setMinutes] = useState((new Date().getTime() - 3600000)/1000/60);
  const [enableLastHalfHour, setEnableLastHalfHour] = useState(false);
  const [enableLastDay, setEnableLastDay] = useState(false);
  const [enableLastWeek, setEnableLastWeek] = useState(false);

  const changeName = async () => {
    await axios.put("/api/stations/name", {
      node_id: stationId,
      name: newName,
    });
  };

  const override = async () => {
    await axios.post("/api/on", { brightness, id: stationId });
  };

  const removeOverride = async () => {
    await axios.post("/api/off", { id: stationId });
  };

  useEffect(() => {
    const fetchStation = async () => {
      const { data } = await axios.get(
        `/api/stations/${stationId}`
      );

      setStation(data);
      if (!editing) setNewName(data.name);

      const { data: telemetry } = await axios.get(
        `/api/stations/${stationId}/telemetry`
      );

      setTelemetry(telemetry);
      if (telemetry.length > 0) {
        const lastReading = new Date(telemetry[telemetry.length - 1].timestamp).getTime() + 3600000;
        setEnableLastHalfHour(lastReading > new Date().getTime() - 30*60*1000);
        setEnableLastDay(lastReading > new Date().getTime() - 24*60*60*1000);
        setEnableLastWeek(lastReading > new Date().getTime() - 7*24*60*60*1000);
      }
    };

    fetchStation().catch(console.error);
    const interval = setInterval(fetchStation, 2000);
    return () => clearInterval(interval);
  }, [stationId, editing]);

  return (
    <div>
      {station && (
        <>
          <Typography variant="h2">
            {editing ? (
              <TextField
                value={newName}
                onChange={(event: any) => setNewName(event.target.value)}
              />
            ) : (
              station.name
            )}
            <IconButton
              onClick={() => {
                if (editing) changeName();
                setEditing((value) => !value);
              }}
            >
              <Edit />
            </IconButton>
          </Typography>
          <Typography variant="h4">Id: {station.node_id}</Typography>
          <Typography variant="h4">
            Last read on {new Date(station.last_read).toLocaleString()}
          </Typography>
          <Typography variant="h4">
            Station is {station.locked ? "locked" : "not locked"}
          </Typography>
          <Typography variant="h4">
            Current brightness: {station.brightness}
          </Typography>

          <Paper elevation={3}>
            <Stack spacing={4} sx={{ my: 6 }} padding={6} textAlign="center">
              <Typography variant="h4">
                Brightness Override: {brightness}%
              </Typography>
              <Stack spacing={2} direction="row" alignItems="center">
                <Slider
                  aria-label="Override brightness"
                  defaultValue={0}
                  step={1}
                  marks
                  min={0}
                  max={100}
                  valueLabelDisplay="auto"
                  onChange={(_, value) => setBrightness(value as number)}
                />

                <Button variant="contained" onClick={() => override()}>
                  Override Brightness
                </Button>
              </Stack>

              <Button
                variant="contained"
                color="secondary"
                onClick={() => removeOverride()}
              >
                Remove override
              </Button>
            </Stack>
          </Paper>
          <Container align="right">
            <Button align="right" variant="contained" disabled={minutes == 30 || !enableLastHalfHour} onClick={() => setMinutes(30)}>
              30 minutes
            </Button>
            <Button variant="contained" disabled={minutes == 1440 || !enableLastDay} onClick={() => setMinutes(1440)}>
              1 day
            </Button>
            <Button variant="contained" disabled={minutes == 10080 || !enableLastWeek} onClick={() => setMinutes(10080)}>
              1 week
            </Button>
            <Button variant="contained" disabled={minutes > 10080} onClick={() => setMinutes(new Date().getTime()/1000/60)}>
              All
            </Button>
          </Container>
          {telemetry && (
          <Paper elevation={3} style={{ marginBottom: 20, padding: 4 }}>
          <ReactApexChart options={{
              chart: {
                type: 'area',
                height: 350,
                zoom: {
                  type: 'x',
                  enabled: true,
                  autoScaleYaxis: true
                },
              },
              yaxis: {
                title: {
                  text: 'brightness(%)'
                },
                tickAmount: 10,
                max: 100,
              },
              xaxis: {
                categories: telemetry.filter((data) => new Date(data.timestamp).getTime()+3600000 > new Date().getTime() - minutes*60*1000).map((data) => new Date(data.timestamp).getTime()+3600000*2),
                type: 'datetime',
              },
              stroke: {
                curve: 'stepline',
              },
              dataLabels: {
                enabled: false
              },
              title: {
                text: 'Brightness intensity history',
                align: 'left'
              }
            }} series= {[{
              name: 'Brightness',
              data: telemetry.filter((data) => new Date(data.timestamp).getTime()+3600000 > new Date().getTime() - minutes*60*1000).map((data) => data.brightness)
            }] }
            type="line" height={350} />
            </Paper>
            )}
          
          {telemetry && (
            <TableContainer component={Paper}>
              <Table aria-label="simple table">
                <TableHead>
                  <TableRow>
                    <TableCell>Time</TableCell>
                    <TableCell align="right">Brightness Level</TableCell>
                    <TableCell align="right">Locked</TableCell>
                  </TableRow>
                </TableHead>
                <TableBody>
                  {telemetry.map((data) => (
                    <TableRow
                      key={data.timestamp}
                      sx={{ "&:last-child td, &:last-child th": { border: 0 } }}
                    >
                      <TableCell component="th" scope="row">
                        {new Date(data.timestamp).toLocaleString("pt-PT", {timeZone: "Europe/Madrid"})}
                      </TableCell>

                      <TableCell align="right">{data.brightness}</TableCell>
                      <TableCell align="right">
                        {data.locked ? "Yes" : "No"}
                      </TableCell>
                    </TableRow>
                  ))}
                </TableBody>
              </Table>
            </TableContainer>
          )}
        </>
      )}
    </div>
  );
};

export default StationPage;

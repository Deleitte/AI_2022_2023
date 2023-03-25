import { Edit } from "@mui/icons-material";
import {
  Button,
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
import { route } from "preact-router";
import { useEffect, useState } from "preact/hooks";
import Telemetry from "src/models/telemetry";

import Station from "../models/station";

interface Props {
  stationId: number;
}

// Note: `user` comes from the URL, courtesy of our router
const StationPage = ({ stationId }: Props) => {
  const [brightness, setBrightness] = useState<number>(0);
  const [station, setStation] = useState<Station>();
  const [telemetry, setTelemetry] = useState<Telemetry[]>();
  const [editing, setEditing] = useState(false);
  const [newName, setNewName] = useState("");

  const changeName = async () => {
    await axios.put("http://localhost:8000/stations/name", {
      node_id: stationId,
      name: newName,
    });
  };

  const override = async () => {
    await axios.post("http://localhost:8000/on", { brightness, id: stationId });
  };

  const removeOverride = async () => {
    await axios.post("http://localhost:8000/off", { id: stationId });
  };

  useEffect(() => {
    const fetchStation = async () => {
      const { data } = await axios.get(
        `http://localhost:8000/stations/${stationId}`
      );

      setStation(data);
      if (!editing) setNewName(data.name);

      const { data: telemetry } = await axios.get(
        `http://localhost:8000/stations/${stationId}/telemetry`
      );

      setTelemetry(telemetry);
    };

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
                  step={25}
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
                        {new Date(data.timestamp).toLocaleString()}
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

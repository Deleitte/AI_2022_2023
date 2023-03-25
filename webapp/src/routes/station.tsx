import { Button, List, Paper, Slider, Stack, Typography } from "@mui/material";
import axios from "axios";
import { h, Fragment } from "preact";
import { route } from "preact-router";
import { useEffect, useState } from "preact/hooks";

import Station from "../models/station";

interface Props {
  stationId: number;
}

// Note: `user` comes from the URL, courtesy of our router
const StationPage = ({ stationId }: Props) => {
  const [brightness, setBrightness] = useState<number>(0);
  const [station, setStation] = useState<Station>();

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
      console.log(data);
      setStation(data);
    };

    fetchStation().catch(() => route("/"));
  }, [stationId]);

  return (
    <div>
      {station && (
        <>
          <Typography variant="h2">{station.name}</Typography>
          <Typography variant="h4">Id: {station.node_id}</Typography>
          <Typography variant="h4">
            Last read on {new Date(station.last_read).toLocaleString()}
          </Typography>
          <Typography variant="h4">
            Station is {station.locked ? "locked" : "not locked"}
          </Typography>

          <Paper elevation={3}>
            <Stack spacing={4} sx={{ mb: 1, mt: 6 }} padding={6} textAlign="center">
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
        </>
      )}
    </div>
  );
};

export default StationPage;

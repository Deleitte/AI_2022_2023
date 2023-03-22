import { h } from "preact";
import axios from "axios";
import { Button, Paper, Slider, Stack, Typography } from "@mui/material";
import { useState } from "preact/hooks";

const Home = () => {
  const [brightness, setBrightness] = useState<number>(0);

  const override = async () => {
    await axios.post("http://localhost:8000/on", { brightness });
  };

  const removeOverride = async () => {
    await axios.post("http://localhost:8000/off");
  };

  return (
    <Paper elevation={3}>
      <Stack spacing={4} sx={{ mb: 1 }} padding={6} textAlign="center">
        <Typography variant="h4">Brightness Override: {brightness}%</Typography>
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
  );
};

export default Home;
